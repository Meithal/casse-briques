//
// Created by hurin on 17/10/2022.
//

#include "stdio.h"

#include "signal.h"
#include <stdbool.h>

#include <winsock2.h>
#include "dirent.h"

#include "common/network/winsock/winsock_utils.h"


#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

#include "server.h"
#include "common/game_rules/gameRules.h"
#include "client/cli/askIntInput.h"




static void startMap(board * board);
DWORD WINAPI threadClient(LPVOID sd_);
DWORD WINAPI threadServeur(LPVOID sd_);

void intHandler(int val);
static int showAvailableMaps(char * folder);


HANDLE consoleWriteMutex;


int main()
{
    int serverThreadIdx = 0;
    HANDLE serverThreads[20];

    /* On devrait faire closesocket(sock); puis WSACleanup(); mais puisqu'on a entré une boucle infinie ... */

    errno = 0;
    signal(SIGINT, intHandler);
    if(errno) {
        perror(strerror(errno));
    }

#ifdef _WIN32
    setupConsoleForUnicode();
    loadCharmap();
#endif
    consoleWriteEvent = CreateEvent(
            NULL, TRUE, FALSE, TEXT("WaitConsole")
    );

    startWinsock();

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

            HANDLE thr = CreateThread(
                    NULL, 0,
                    threadServeur, NULL, 0, &nThreadId);
            if (thr == NULL) {
                _tprintf(_T("failed to create server thread: %d"), GetLastError());
            }
            serverThreads[serverThreadIdx++] = thr;
            _tprintf(_T("Thread serveur crée: thid %d\n"), nThreadId);
            fflush(stdout);

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
                    _tprintf(_T("Thread failed %"W"s\n"), friendlyErrorMessage(GetLastError()));
                }
            }
            break;
        }
        case 3:
            goto exit;
    }
} while (1);
    exit:

    shutdownAsked = 1;

    for (int i = 0; i < serverThreadIdx; i++) {
        CloseHandle(serverThreads[i]);
    }
    CloseHandle(consoleWriteMutex);
    CloseHandle(consoleWriteEvent);
    stopWinsock();

    return 0;
}


static void startMap(board * board)
{
    _TCHAR buf[0x100] = {0};
    mapView(0x100, buf, board);
    _putts(buf);
    fflush(stdout);

    SOCKET s;
    fflush(stdout);

    startServer(&s, (LPTHREAD_START_ROUTINE) threadClient, &board->serverPort);
    closeServer(&s);
}

DWORD WINAPI threadServeur(LPVOID sd_)
{
    int retVal = 0;

    int mapsNumber = showAvailableMaps("assets/maps");
    int map = askIntInput(_T("Quelle carte ?"), 1, mapsNumber);

    char buf[256];
    sprintf(buf, "assets/maps/grille%d.txt", map);
    board board;
    loadMap(buf, &board);
    int ias = askIntInput(_T("Combien de IA vont jouer dans la map ?"), 0, board.nb_players);
    setAIPlayers(ias, &board);
    board.serverPort = 41480;

    startMap(&board);

    return retVal;
}

DWORD WINAPI threadClient(LPVOID sd_) {
    int nRetval = 0;
    SOCKET sd = (SOCKET)sd_;

    if (!connectionClient(sd)) {
        _putts(_T("Erreur avec le client"));
        nRetval = 3;
    }

    _putts(_T("Fermeture connection avec client..."));
    if (shutdownConnection(sd)) {
        _tprintf(_T("Connection is down."));
    }
    else {
        _ftprintf(stderr, _T("Connection shutdown failed\n"));
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
        wchar_t longName[MAX_PATH] = {0};
        mbtowc(longName, desc->d_name, desc->d_namlen);

        _tprintf(_T("%d. %"W"s\n%"W"s"), ret+1, longName, bufOut);
        ret++;
    }

    cleanup:
    if(fol) closedir(fol);

    return ret;
}

void intHandler(int val) {
    _tprintf(_T("sigint recu %d"), val);
    fflush(stdout);
}