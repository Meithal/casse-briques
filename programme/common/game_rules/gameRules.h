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

int loadMap(char* path, struct board * board);
void unloadMap(struct board * board);
int setAIPlayers(int number, struct board * board);
int getPlacesRestantes(struct hosted_game * hostedGame);
struct hosted_game *hostedGameFromIdx(int hostedGamesMax, struct hosted_game games[], int idx);
struct player * playerAtPosition(struct board*board, int y, int x);
_Bool canMoveAt(struct board*board, struct player*player, struct vec2dir to);
struct bombe * bombAt(struct board*board, int y, int x);
_Bool canLayBombAt(struct board*board, struct player*player);
void layBomb(struct board*board, struct player*player);

void updateGameState(struct board*board);
void computeDeflagration(int rows, int cols, _Bool (*map)[rows][cols], struct board*board);
void freeDeflagration(int rows, int cols, _Bool (*map)[*][*]);
_Bool isInDeflagration(struct board*board, int rows, int cols, _Bool (*map)[rows][cols], int y, int x);
_Bool shouldExplodeBomb(struct bombe*bomb);

extern int charmap[0xff];
extern const int wide;


#endif //CASSES_BRIQUES_GAMERULES_H
