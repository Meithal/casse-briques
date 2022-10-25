//
// Created by maxim on 11/10/2022.
//

#include "stdio.h"
#include "string.h"

#include "common/structures.h"
#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

int main () {
    field vide = {
            .visual = 32,
            .txt = _T(' '),
            .destructible = 0,
    };
    field bricks = {
            .visual = 176,
            .txt = _T('m'),
            .destructible = 1,
    };
    field mur = {
            .visual = 219,
            .txt = _T('x'),
            .destructible = 0,
    };
#ifdef _WIN32
    puts("win32 detected");
    EnableVTMode();
    SetupConsoleForUnicode();
    LoadCharmap();
#endif
#ifdef _UNICODE
    const int wide = 1;
#define W "l"
#else
    const int wide = 0;
#define W ""
#endif
    _putts(_TEXT("tesr"));
    fflush(stdout);
    _putts(_T("Biénvenue dans le casse briques."));
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
                _tprintf(_T("%"W"c"),wide ? charmap[mur.visual] : mur.visual);
            } else if (lettre==bricks.txt) {
                _tprintf(_T("%"W"c"),wide ? charmap[bricks.visual] : bricks.visual);
            } else if (lettre==vide.txt) {
                _tprintf(_T("%"W"c"),wide ? charmap[vide.visual] : vide.visual);
            } else if (lettre=='p') {
                _tprintf(_T("p"));
            } else {
                _tprintf(_T("\n"));
            }
            lettre = fgetc(f);
        }
    }
}
