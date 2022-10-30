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
DWORD WINAPI ThreadServeur(LPVOID sd_);

void intHandler(int dummy);
static int showAvailableMaps(char * folder);

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

#ifdef _WIN32
    SetupConsoleForUnicode();
    LoadCharmap();
#endif

    _putts("1. Voir les parties en cours.");
    _putts("2. Host une nouvelle partie.");

    int input = askIntInput(1, 2);

    switch (input) {
        case 1:
            // todo
        case 2:
        default: {
            int mapsNumber = showAvailableMaps("assets/maps");
            int map = askIntInput(1, mapsNumber);

            char buf[256];
            sprintf(buf, "assets/maps/grille%d.txt", map);
            board board;
            loadMap(buf, &board);
            _putts("Combien de IA vont jouer dans la map ?");
            int ias = askIntInput(0, board.nb_players);

            startMap(&board);
        }

    }

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

    StartServer(&s, (LPTHREAD_START_ROUTINE) ThreadServeur, 41480);
    CloseServer(&s);
    StopWinsock();
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


DWORD WINAPI ThreadServeur(LPVOID sd_) {
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
//        _TCHAR buf[0x100] = {0};
        mapView(0x100, buf, &board);
       // _putts(buf);

        _tprintf(_T("%d. %s\n%s"), ret+1, desc->d_name, buf);
        ret++;
    }

    cleanup:
    if(fol) closedir(fol);

    return ret;
}

void intHandler(int val) {
    return;
}