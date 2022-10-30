//
// Created by maxim on 11/10/2022.
//

#include "stdio.h"

#ifdef _WIN32
#include "conio.h"
#else
#include "client/cli/linux_compatibility/conio_polyfill.h"
#endif


#include "common/structures.h"
#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

extern int charmap[0xff];
extern const int wide;


int main () {
#ifdef _WIN32
    puts("win32 detected");
    EnableVTMode();
    SetupConsoleForUnicode();
    LoadCharmap();
#endif
    _putts(_TEXT("tesr"));
    fflush(stdout);
    _putts(_T("Bienvenue dans le casse briques."));
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

    fclose(f);
}
