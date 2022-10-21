//
// Created by maxim on 11/10/2022.
//

#include "stdio.h"
#include "string.h"
#include <wchar.h>
#include <locale.h>

#include "common/structures.h"
#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

int main () {
    setlocale(LC_CTYPE, "");
    field vide = {
            .visual = 0x0020,
            .txt = _T(' '),
            .destructible = 0,
    };
    field bricks = {
            .visual = 0x2591,
            .txt = _T('m'),
            .destructible = 1,
    };
    field mur = {
            .visual = 0x2588,
            .txt = _T('x'),
            .destructible = 0,
    };
#ifdef _WIN32
    EnableVTMode();
    SetupConsoleForUnicode();
#endif
    _putts(_TEXT("tesr"));
    fflush(stdout);
    _putts(L"Bienvenueé dans le casse briques.");
    _putts(_TEXT("Appuyez sur une touche pour continuer."));
    getchar();
    _putts(_TEXT("\033[2J\033[H"));

    FILE *f = fopen("assets/maps/grille1.txt","r");
    if (f == NULL) {
        printf("file empty");
    } else {
        fseek(f, 7, SEEK_CUR);
        int column;
        fscanf(f, "%d",&column);
        fseek(f, 6, SEEK_CUR);
        int row;
        fscanf(f, "%d",&row);
        fseek(f, 2, SEEK_CUR);

        char lettre = fgetc(f);
        while (lettre!=EOF) {
            if (lettre==mur.txt) {
                wprintf(L"%lc", mur.visual);
            } else if (lettre==bricks.txt) {
                wprintf(L"%lc", bricks.visual);
            } else if (lettre==vide.txt) {
                wprintf(L"%lc", vide.visual);
            } else if (lettre=='p') {
                _tprintf(_T("p"));
            } else {
                _tprintf(_T("\n"));
            }
            lettre = fgetc(f);
        }
    }
}