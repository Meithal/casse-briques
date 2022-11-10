//
// Created by hurin on 17/10/2022.
//

#include "stdio.h"

#include "signal.h"

#include <winsock2.h>
#include "dirent.h"
#include "conio.h"
#include "json.h"

#include "common/network/winsock/winsock_utils.h"


#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

#include "common/game_rules/gameRules.h"
#include "client/cli/grilleDisplay.h"
#include "client/cli/askIntInput.h"
#include "client/cli/client.h"

DWORD WINAPI threadServerListenClient(LPVOID sd_);
DWORD WINAPI threadServeur(LPVOID phosted_game);
void onConnectCallback(SOCKET sock);

void intHandler(int val);
static int showAvailableMaps(char * folder);

HANDLE consoleWriteMutex;

int main()
{
    int serverThreadIdx = 0;
    HANDLE serverThreads[20];

    int hostedGamesIdx = 0;
    hosted_game hostedGames[20];
    int boardsIdx = 0;
    board boards[20];

    /* On devrait faire closesocket(sock); puis WSACleanup(); mais puisqu'on a entré une boucle infinie ... */

    errno = 0;
    signal(SIGINT, intHandler);
    if(errno) {
        perror(strerror(errno));
    }

#ifdef _WIN32
    enableVtMode();
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
        case 1: {
            _putts(_T("Parties en cours :"));
            for (int i = 0; i < hostedGamesIdx; ++i) {
                _TCHAR bufOut[0x100] = {0};
                mapView(0x100, bufOut, hostedGames[i].board, NULL);

                _tprintf(_T("%d. \033[33mPlaces restantes : %d\033[0m\n%"W"s"),
                    i+1,
                    getPlacesRestantes(&hostedGames[i]),
                    bufOut
                );
            }

            _putts(_T("Quelle partie rejoindre ?"));

            int found = 0;
            for (int i = 0; i < hostedGamesIdx; ++i) {
                if(!getPlacesRestantes(&hostedGames[i])) {
                    continue;
                }
                _tprintf(_T("%d. Joindre la partie %d\n"), found+1, i+1);
                found++;
            }
            _tprintf(_T("%d. Revenir à l'accueil\n"), found+1);

            input = askIntInput(NULL, 1, found+1);
            if(input == found+1) {
                break;
            } else {
                CreateThread(
                        NULL, 0,
                        threadClient, hostedGameFromIdx(hostedGamesIdx, hostedGames, input),
                        0, NULL
                );
                ResetEvent(consoleWriteEvent);
                WaitForSingleObject(consoleWriteEvent, INFINITE);
            }
            break;
        }
        case 2:
        default: {
            DWORD nThreadId;

            boards[boardsIdx++] = (board) {0};
            hostedGames[hostedGamesIdx++] = (hosted_game) {.board = &boards[boardsIdx-1]};

            HANDLE thr = CreateThread(
                    NULL, 0,
                    threadServeur, &hostedGames[hostedGamesIdx-1], 0, &nThreadId);
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

DWORD WINAPI threadServeur(LPVOID phosted_game)
{
    int retVal = 0;
    hosted_game * hostedGame = phosted_game;
    struct board * board = hostedGame->board;

    int mapsNumber = showAvailableMaps("assets/maps");
    int map = askIntInput(_T("Quelle carte ?"), 1, mapsNumber);

    char buf[256];
    sprintf(buf, "assets/maps/grille%d.txt", map);
    loadMap(buf, board);
    int ias = askIntInput(_T("Combien de IA vont jouer dans la map ?"), 0, board->nb_players);
    setAIPlayers(ias, board);
    hostedGame->serverPort = 41480;
    hostedGame->mapNumber = map;

    SOCKET s;
    startServer(
            &s,
            (LPTHREAD_START_ROUTINE) threadServerListenClient,
            &hostedGame->serverPort
    );
    closeServer(&s);

    return retVal;
}

void onConnectCallback(SOCKET sock)
{
    int nTemp = send(sock, "wru",4, 0); // who are you
    if (nTemp > 0) {
//        _tprintf(_T("Envoyé %d bytes au client.\n"), nTemp);
    }
    else if (nTemp == SOCKET_ERROR) {
        return;
    }
    else {
        // Client closed connection before we could reply to
        // all the data it sent, so bomb out early.
        _putts(_T("Pair a fermé la connexion de manière inattendue !"));
        return;
    }

//    int nReadBytes;
//    char responseBuf[10] = {0};
//    wchar_t wideBuf[10] = {0};
//    do {
//        nReadBytes = recv(sock, responseBuf, 10, 0);
//        mbtowc(wideBuf, responseBuf, nReadBytes);
//        _putts(wideBuf);
//    } while (nReadBytes != 0);
}

DWORD WINAPI threadServerListenClient(LPVOID sd_) {
    int nRetval = 0;
    SOCKET sd = (SOCKET)sd_;

    onConnectCallback(sd);

    if (!connectionClient(sd)) {
        _tprintf(_T("Erreur avec le client %"W"s\n"), friendlyErrorMessage(WSAGetLastError()));
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
    DIR * fol = opendir(folder);
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
        mapView(0x100, bufOut, &board, NULL);
       // _putts(buf);
        wchar_t longName[MAX_PATH] = {0};
        mbstowcs(longName, desc->d_name, desc->d_namlen);

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