//
// Created by hurin on 05/11/2022.
//

#include "client.h"


static char *afficheJoueurs(int currentTile, struct player *player) {
    return "p";
}


DWORD WINAPI threadClient(LPVOID phosted_game) {
    hosted_game *hostedGame = phosted_game;
    _putts(_T("Bienvenue dans la partie en cours"));
    _putts(_T("Appuyez sur les flèches pour vous déplacer, q pour quitter."));

    _tprintf(_T("\033[2J"));

    while (1) {
        Sleep(200);
        _tprintf(_T("\033[H"));

        _TCHAR bufOut[0x100] = {0};
        mapView(0x100, bufOut, hostedGame->board, afficheJoueurs);
        _putts(bufOut);

        if (kbhit()) {
            int ch = getch();

            if (ch == 'q') {
                break;
            }
            _tprintf(_T("%d"), ch);
        }
    }

    _putts(_T(""));
    SetEvent(consoleWriteEvent);
    return 0;
}
