//
// Created by maxim on 11/10/2022.
//

#include "stdio.h"
#include "string.h"

#include "common/structures.h"

int main () {
    field vide = {
            .visual = 32,
            .txt = ' ',
            .destructible = 0,
    };
    field bricks = {
            .visual = 176,
            .txt = 'm',
            .destructible = 1,
    };
    field mur = {
            .visual = 219,
            .txt = 'x',
            .destructible = 0,
    };

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
                printf("%c",mur.visual);
            } else if (lettre==bricks.txt) {
                printf("%c",bricks.visual);
            } else if (lettre==vide.txt) {
                printf("%c",vide.visual);
            } else if (lettre=='p') {
                printf("p");
            } else {
                printf("\n");
            }
            lettre = fgetc(f);
        }
    }
}