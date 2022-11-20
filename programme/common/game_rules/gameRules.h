//
// Created by Utilisateur on 26/10/2022.
//

#ifndef CASSES_BRIQUES_GAMERULES_H
#define CASSES_BRIQUES_GAMERULES_H

#include "tchar.h"

#include "common/structures.h"

#define GAME_CLIENT_INDEX_UNASSIGNED (-1)


struct vec2dir {
    int x;
    int y;
};

int loadMap(char* path, board * board);
void unloadMap(board * board);
int setAIPlayers(int number, board * board);
int getPlacesRestantes(hosted_game * hostedGame);
hosted_game *hostedGameFromIdx(int hostedGamesMax, hosted_game games[], int idx);
player * playerAtPosition(board*board, int y, int x);
_Bool canMoveAt(board*board, player*player, struct vec2dir to);


extern int charmap[0xff];
extern const int wide;


#endif //CASSES_BRIQUES_GAMERULES_H
