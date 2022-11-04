//
// Created by hurin on 17/10/2022.
//

#include "stdio.h"

#include "signal.h"
#include <stdbool.h>

#include <winsock2.h>
#include"dirent.h"

#include "common/network/winsock/winsock_utils.h"


#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

#include "server.h"
#include "common/game_rules/gameRules.h"
#include "client/cli/askIntInput.h"

extern const int wide;
extern int charmap[0xff];

bool ConnectionClient(SOCKET sd);
static void startMap(board * board);
DWORD WINAPI ThreadClient(LPVOID sd_);
DWORD WINAPI ThreadServeur(LPVOID sd_);

void intHandler(int dummy);
static int showAvailableMaps(char * folder);

int serverThreadIdx = 0;
HANDLE serverThreads[20];

HANDLE consoleWriteMutex;


int main()
{
//    WSADATA WSAData;
//    SOCKET sock;
//    SOCKET csock;
//    SOCKADDR_IN sin;
//    SOCKADDR_IN csin;
//    WSAStartup(MAKEWORD(2,0), &WSAData);
//    sock = socket(AF_INET, SOCK_STREAM, 0);
//    sin.sin_addr.s_addr = INADDR_ANY;
//    sin.sin_family = AF_INET;
//    sin.sin_port = htons(23);
//    bind(sock, (SOCKADDR *)&sin, sizeof(sin));
//    listen(sock, 0);
//    while(1) /* Boucle infinie. Exercice : améliorez ce code. */
//    {
//        int sinsize = sizeof(csin);
//        if((csock = accept(sock, (SOCKADDR *)&csin, &sinsize)) != INVALID_SOCKET)
//        {
//            send(csock, "Hello world!\r\n", 14, 0);
//            closesocket(csock);
//        }
//    }
    /* On devrait faire closesocket(sock); puis WSACleanup(); mais puisqu'on a entré une boucle infinie ... */

    signal(SIGINT, intHandler);
    HANDLE thr;
#ifdef _WIN32
    SetupConsoleForUnicode();
    LoadCharmap();
#endif
    consoleWriteEvent = CreateEvent(
            NULL, TRUE, FALSE, TEXT("WaitConsole")
    );

    do {
    _putts(_T("1. Voir les parties en cours."));
    _putts(_T("2. Host une nouvelle partie."));
    _putts(_T("3. Arrêter le serveur."));

    int input = askIntInput(NULL, 1, 3);

    switch (input) {
        case 1:
            // todo
        case 2:
        default: {
            DWORD nThreadId;

            thr = CreateThread(
                    NULL, 0,
                    ThreadServeur, NULL, 0, &nThreadId);
            if (thr == NULL) {
                _tprintf(_T("failed to create server thread: %d"), GetLastError());
            }
            serverThreads[serverThreadIdx++] = thr;
            _tprintf(_T("Thread serveur crée: thid %d\n"), nThreadId);
            fflush(stdout);

//            consoleWriteMutex = CreateMutex(
//                    NULL, FALSE, _T("Local\\ConsoleWriteMutex")
//            );


//            if(consoleWriteMutex == NULL) {
//                _tprintf(_T("Error creating mutex %"W"s"), FriendlyErrorMessage(GetLastError()));
//            }
//            ReleaseMutex(consoleWriteMutex);
//            Sleep(200);

            ResetEvent(consoleWriteEvent);
            DWORD wait = WaitForSingleObject(consoleWriteEvent, INFINITE);
            switch (wait) {
                case WAIT_ABANDONED: {
                    _putts(_T("Owning thread terminated"));
                    break;
                }
                case WAIT_OBJECT_0: {
                    _putts(_T("Thread signaled"));
                    break;
                }
                case WAIT_TIMEOUT: {
                    _putts(_T("Timeout elapsed"));
                    break;
                }
                default:
                case WAIT_FAILED: {
                    _tprintf(_T("Thread failed %"W"s\n"), FriendlyErrorMessage(GetLastError()));
                }
            }
            break;
        }
        case 3:
            goto exit;
    }
} while (1);
    exit:

    CloseHandle(thr);
    CloseHandle(consoleWriteMutex);
    CloseHandle(consoleWriteEvent);

    return 0;
}


static void startMap(board * board)
{
    _TCHAR buf[0x100] = {0};
    mapView(0x100, buf, board);
    _putts(buf);
    fflush(stdout);

    StartWinsock();
    SOCKET s;
    fflush(stdout);

    StartServer(&s, (LPTHREAD_START_ROUTINE) ThreadClient, 41480);
    CloseServer(&s);
    StopWinsock();
}

DWORD WINAPI ThreadServeur(LPVOID sd_)
{
    int retVal = 0;
    WaitForSingleObject(consoleWriteMutex, INFINITE);

    int mapsNumber = showAvailableMaps("assets/maps");
    int map = askIntInput(_T("Quelle carte ?"), 1, mapsNumber);

    char buf[256];
    sprintf(buf, "assets/maps/grille%d.txt", map);
    board board;
    loadMap(buf, &board);
    int ias = askIntInput(_T("Combien de IA vont jouer dans la map ?"), 0, board.nb_players);
    setAIPlayers(ias, &board);

    startMap(&board);

    return retVal;
}


bool ConnectionClient(SOCKET sd) {
    // Read data from client
    char acReadBuffer[K_BUFFER_SIZE];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, K_BUFFER_SIZE, 0);
        if (nReadBytes > 0) {
            _tprintf(_T("Received %d bytes from client.\n"), nReadBytes);

            int nSentBytes = 0;
            while (nSentBytes < nReadBytes) {
                int nTemp = send(sd, acReadBuffer + nSentBytes,
                                 nReadBytes - nSentBytes, 0);
                if (nTemp > 0) {
                    _tprintf(_T("Sent %d bytes back to client.\n"), nTemp);
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR) {
                    return false;
                }
                else {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    _putts(_T("Peer unexpectedly dropped connection!"));
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR) {
            return false;
        }
    } while (nReadBytes != 0);

    _putts(_T("Connection closed by peer."));
    return true;
}


DWORD WINAPI ThreadClient(LPVOID sd_) {
    int nRetval = 0;
    SOCKET sd = (SOCKET)sd_;

    if (!ConnectionClient(sd)) {
        printf("Erreur avec le client\n");
        nRetval = 3;
    }

    puts("Fermeture connection avec client...");
    if (ShutdownConnection(sd)) {
        printf("Connection is down.");
    }
    else {
        fprintf(stderr, "Connection shutdown failed\n");
        nRetval = 3;
    }

    return nRetval;
}

static int showAvailableMaps(char * folder) {

    int ret = 0;
    DIR * fol;
    fol = opendir(folder);
    if(fol == NULL) {
        perror(strerror(errno));
        goto cleanup;
    }
    struct dirent * desc = NULL;
    while((desc = readdir(fol)) != NULL) {
        if(desc->d_name[0] == '.') {
            continue;
        }

        char buf[0x100];
        sprintf(buf, "%s/grille%d.txt", folder, ret +1);
        board board;

        loadMap(buf, &board);
        _TCHAR bufOut[0x100] = {0};
        mapView(0x100, bufOut, &board);
       // _putts(buf);

        _tprintf(_T("%d. %s\n%"W"s"), ret+1, desc->d_name, bufOut);
        ret++;
    }

    cleanup:
    if(fol) closedir(fol);

    return ret;
}

void intHandler(int val) {
    _tprintf(_T("sigint recu %d"), val);
    fflush(stdout);
    return;
}