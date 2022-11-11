//
// Created by maxim on 11/10/2022.
//

#include "stdio.h"
#include <locale.h>

#ifdef _WIN32
#include "conio.h"
#else
#include "client/cli/linux_compatibility/conio_polyfill.h"
#endif


#include "common/structures.h"
#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

static int vider_tampon(FILE *fp)
{
    /*
     * Vide les données en attente de lecture du flux spécifié.
     */

    int c;

    do
        c = fgetc(fp);
    while (c != '\n' && c != EOF);

    return ferror(fp) ? 0 : 1;
}


int main () {
    setlocale(LC_CTYPE, "");
    player playerOne = {
            .visual = '1',
            .position = 0,
    };
    player playerTwo = {
            .visual = '2',
            .position = 0,
    };
    player playerThree = {
            .visual = '3',
            .position = 0,
    };
    player playerFour = {
            .visual = '4',
            .position = 0,
    };

#ifdef _WIN32
    puts("win32 detected");
    enableVtMode();
    setupConsoleForUnicode();
    loadCharmap();
#endif
    fflush(stdout);
    _putts(_T("Bienvenue dans le casse briques."));
    _putts(_TEXT("Appuyez sur une touche pour continuer."));
    getchar();
    _putts(_TEXT("\033[2J\033[H"));

    FILE *f = fopen("assets/maps/grille1.txt","r");
    if (f == NULL) {
        printf("file empty");
    } else {
        fseek(f, 6, SEEK_CUR);
        int bomb;
        fscanf(f, "%d",&bomb);
        fseek(f, 9, SEEK_CUR);
        int column;
        fscanf(f, "%d",&column);
        fseek(f, 6, SEEK_CUR);
        int row;
        fscanf(f, "%d",&row);
        fseek(f, 2, SEEK_CUR);

        // On print une première fois la carte
        char lettre = fgetc(f);
        int numberPlayer = 1;
        int position = 0;
        while (lettre!=EOF) {
            if (lettre==mur.txt) {
                _tprintf(_T("%"W"c"),wide ? charmap[mur.visual] : mur.visual);
                position++;
            } else if (lettre==bricks.txt) {
                _tprintf(_T("%"W"c"),wide ? charmap[bricks.visual] : bricks.visual);
                position++;
            } else if (lettre==vide.txt) {
                _tprintf(_T("%"W"c"),wide ? charmap[vide.visual] : vide.visual);
                position++;
            } else if (lettre=='p') {
                position++;
                if (numberPlayer==1) {
                    playerOne.position = position;
                    wprintf(L"%lc", playerOne.visual);
                } else if (numberPlayer==2) {
                    playerTwo.position = position;
                    wprintf(L"%lc", playerTwo.visual);
                } else if (numberPlayer==3) {
                    playerThree.position = position;
                    wprintf(L"%lc", playerThree.visual);
                } else if (numberPlayer==4) {
                    playerFour.position = position;
                    wprintf(L"%lc", playerFour.visual);
                }
                numberPlayer++;
            } else {
                _tprintf(_T("\n"));
            }
            lettre = fgetc(f);
        }


        // Les déplacements
        int turn = 1;
        while (1) {
            _putts(_TEXT("\nAppuyez sur zqsd pour vous déplacer"));
            vider_tampon(stdin);
            char movement = getchar();


            int positionSave;
            if (turn == 1) {
                positionSave = playerOne.position;
                if (movement=='d') {
                    playerOne.position++;
                } else if (movement=='s') {
                    playerOne.position += column;
                } else if (movement=='z') {
                    playerOne.position -= column;
                } else if (movement=='q') {
                    playerOne.position--;
                }
            } else if (turn == 2) {
                positionSave = playerTwo.position;
                if (movement=='d') {
                    playerTwo.position++;
                } else if (movement=='s') {
                    playerTwo.position += column;
                } else if (movement=='z') {
                    playerTwo.position -= column;
                } else if (movement=='q') {
                    playerTwo.position--;
                }
            }




            fseek(f, -(column*row+row*2), SEEK_END);
            lettre = fgetc(f);
            position = 1;
            while (lettre!=EOF) {
                if (position==playerOne.position) {
                    if ((lettre!=vide.txt && lettre!='p') || playerOne.position==playerTwo.position) {
                        playerOne.position = positionSave;
                    }
                    _tprintf(_T("%lc"), playerOne.visual);
                    position++;
                } else if (position==playerTwo.position) {
                    if ((lettre!=vide.txt && lettre!='p') || playerTwo.position==playerOne.position) {
                        playerTwo.position = positionSave;
                    }
                    wprintf(L"%lc", playerTwo.visual);
                    position++;
                } else if (lettre==mur.txt && position!=playerOne.position) {
                    wprintf(L"%lc", mur.visual);
                    position++;
                } else if (lettre==bricks.txt && position!=playerOne.position) {
                    wprintf(L"%lc", bricks.visual);
                    position++;
                } else if (lettre==vide.txt && position!=playerOne.position)  {
                    wprintf(L"%lc", vide.visual);
                    position++;
                } else if (lettre=='p' && position!=playerOne.position) {
                    wprintf(L"%lc", vide.visual);
                    position++;
                } else {
                    _tprintf(_T("\n"));
                }
                lettre = fgetc(f);
            }

            if (turn == 1) {
                turn = 2;
            } else if (turn == 2){
                turn = 1;
            }

        }
    }

    fclose(f);
}
