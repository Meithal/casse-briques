//
// Created by Utilisateur on 20/11/2022.
//

#ifndef CASSES_BRIQUES_CALLBACKS_H
#define CASSES_BRIQUES_CALLBACKS_H

#include "common/structures.h"

void onGameSlotAssigned(struct hosted_game* hostedGame, int slot);
void onDeplacementDemande(struct hosted_game* hostedGame, int direction);
void onBombePoseDemande(struct hosted_game* hostedGame);

#endif //CASSES_BRIQUES_CALLBACKS_H
