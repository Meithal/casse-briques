//
// Created by Utilisateur on 26/10/2022.
//

#ifndef CASSES_BRIQUES_GAMERULES_H
#define CASSES_BRIQUES_GAMERULES_H

#include "tchar.h"

#include "common/structures.h"

int loadMap(char* path, board * board);
void unloadMap(board * board);
void mapView(int size, _TCHAR * buffer, board * board);
int setAIPlayers(int number, board * board);

extern int charmap[0xff];
extern const int wide;


#endif //CASSES_BRIQUES_GAMERULES_H
