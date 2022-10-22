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
    fflush(stdout);
    _putts(L"Bienvenue dans le casse briques.");
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

        char lettre = fgetc(f);
        int numberPlayer = 1;
        int position = 0;
        while (lettre!=EOF) {
            if (lettre==mur.txt) {
                wprintf(L"%lc", mur.visual);
                position++;
            } else if (lettre==bricks.txt) {
                wprintf(L"%lc", bricks.visual);
                position++;
            } else if (lettre==vide.txt) {
                wprintf(L"%lc", vide.visual);
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

        int turn1 = 1;
        int turn2 = 0;
        int turn3 = 0;
        int turn4 = 0;
        while (1) {
            _putts(_TEXT("\nAppuyez sur zqsd pour vous déplacer"));
            char movement = getchar();
            fseek(f, -(column*row+row*2), SEEK_END);
            lettre = fgetc(f);

            char check;
            int save1 = playerOne.position;
            int save2 = playerTwo.position;
            if (movement=='d') {
                if (turn1) {
                    playerOne.position++;
                    int i = 0;
                    while (playerOne.position > column*i) {
                        i++;
                    }
                    i--;
                    fseek(f,playerOne.position+i,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerOne.position+i),SEEK_CUR);
//                    turn1 = 0;
//                    turn2 = 1;
                } else if (turn2) {
                    playerTwo.position++;
                    wprintf(L"%ld\n",playerTwo.position);
                    fseek(f,playerTwo.position,SEEK_CUR);
                    check = fgetc(f);
                    wprintf(L"%c\n",check);
                    fseek(f,-(playerTwo.position),SEEK_CUR);
                    turn1 = 1;
                    turn2 = 0;
                }

            } else if (movement=='s') {
                if (turn1) {
                    playerOne.position += column;
                    int i = 0;
                    while (playerOne.position > column*i) {
                        i++;
                    }
                    i--;
                    fseek(f,playerOne.position+i,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerOne.position+i),SEEK_CUR);
//                    turn1 = 0;
//                    turn2 = 1;
                } else if (turn2) {
                    playerTwo.position += column;
                    fseek(f,playerTwo.position,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerTwo.position),SEEK_CUR);
                    turn1 = 1;
                    turn2 = 0;
                }

            } else if (movement=='q') {
                if (turn1) {
                    playerOne.position--;
                    int i = 0;
                    while (playerOne.position > column*i) {
                        i++;
                    }
                    i--;
                    wprintf(L"p1 : %ld\n",playerOne.position);
                    wprintf(L"i : %ld\n",i);
                    fseek(f,playerOne.position+i,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerOne.position),SEEK_CUR);
//                    turn1 = 0;
//                    turn2 = 1;
                } else if (turn2) {
                    playerTwo.position--;
                    fseek(f,playerTwo.position,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerTwo.position),SEEK_CUR);
                    turn1 = 1;
                    turn2 = 0;
                }

            } else if (movement=='z') {
                if (turn1) {
                    playerOne.position -= column;
                    fseek(f,playerOne.position,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerOne.position),SEEK_CUR);
//                    turn1 = 0;
//                    turn2 = 1;
                } else if (turn2) {
                    playerTwo.position -= column;
                    fseek(f,playerTwo.position,SEEK_CUR);
                    check = fgetc(f);
                    fseek(f,-(playerTwo.position),SEEK_CUR);
                    turn1 = 1;
                    turn2 = 0;
                }

            }


            if (check ==' ' || check =='p') {
                position = 1;
                while (lettre!=EOF) {
                    if (position==playerOne.position) {
                        wprintf(L"%lc", playerOne.visual);
                        position++;
                    } else if (position==playerTwo.position) {
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
            } else {
                _putts(_TEXT("Mouvement impossible, veillez réessayer"));
                playerOne.position = save1;
                playerTwo.position = save2;
                if (turn1) {
                    turn1=0;
                    turn2=1;
                } else if (turn2) {
                    turn2 = 0;
                    turn1 = 1;
                }

                position = 1;
                while (lettre!=EOF) {
                    if (position==playerOne.position) {
                        wprintf(L"%lc", playerOne.visual);
                        position++;
                    } else if (position==playerTwo.position) {
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
            }
        }

    }
}