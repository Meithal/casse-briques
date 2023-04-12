//
// Created by hurin on 05/11/2022.
//

#ifndef CASSES_BRIQUES_GRILLEDISPLAY_H
#define CASSES_BRIQUES_GRILLEDISPLAY_H

#include "tchar.h"
#include "common/structures.h"
#include "common/game_rules/gameRules.h"
#include "client/cli/windows_compatibility/winterm.h"

void mapView(
    int size,
    _TCHAR *buffer,
    struct board *board,
    char *(*displayCallback)(int currentTile, struct player *player)
);
#endif //CASSES_BRIQUES_GRILLEDISPLAY_H
