//
// Created by Utilisateur on 26/10/2022.
//

#include "stdio.h"
#include "stdlib.h"

#include "common/structures.h"
#include "gameRules.h"


//extern const int wide;
//extern int charmap[0xff];


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


int loadMap(char* path, board * board)
{
    FILE* f = fopen(path, "r");

    if(f == NULL) {
        perror(NULL);
        fflush(stderr);

        return 1;
    }

    int bombes;
    int lignes;
    int cols;

    char line[256];
    fgets(line, 256, f);
    sscanf(line, "bombs=%d", &bombes);
    fgets(line, 256, f);
    sscanf(line, "column=%d", &cols);
    fgets(line, 256, f);
    sscanf(line, "row=%d", &lignes);

    #ifdef _MSC_VER
    board->board = malloc(sizeof (tile)* lignes*cols);
    #else
    board->board = malloc(sizeof (tile[lignes][cols]));
    #endif
    board->players = NULL;

//    typedef tile (boardptr)[board->rows][board->cols];

    int players = 0;
    char c;
    for (int j = 0 ; j < lignes ; j++) {
        for (int i = 0; i < cols; i++) {
            fscanf(f, "%c", &c);
            (*board->board)[j*cols+i] = (struct tile) {
                    .type = (c == 'p' || c == vide.txt) ? &vide : c == mur.txt ? &mur : &bricks,
                    .destroyed = 0
            };

        if(c == 'p') {
            void * newPlayers = realloc(board->players, sizeof (struct player) * (players + 1));
            if(newPlayers == NULL) {
                unloadMap(board);
                return 1;
            }
            board->players = newPlayers;
            (*board->players)[players].line = j;
            (*board->players)[players].col = i;
            (*board->players)[players].is_ia = 0;

            players++;
        }
        }
        fgetc(f);
    }

    board->nb_players = players;
    board->rows = lignes;
    board->cols = cols;

//    fclose(f);

    return 0;
}

void unloadMap(board *board) {
    free(board->players);
    free(board->board);
}

int setAIPlayers(int number, board * board)
{
    if(number > board->nb_players) {
        return 0;
    }
    while(number--) {
        (*board->players)[number].is_ia = 1;
    }

    return 1;
}

int getPlacesRestantes(hosted_game * hostedGame)
{
    int restant = 0;
    board * board = hostedGame->board;
    int max = board->nb_players;

    restant = max;

    for(int i = 0; i < max ; i++) {
        player player = (*board->players)[i];
        if(player.is_ia) {
            restant--;
        }
    }

    restant -= hostedGame->nbClients;

    return restant;
}