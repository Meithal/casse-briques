//
// Created by Utilisateur on 20/11/2022.
//

#include "tchar.h"

#include "callbacks.h"
#include "common/game_rules/gameRules.h"

void onGameSlotAssigned(hosted_game* hostedGame, int slot)
{
    _tprintf(_T("onGameSlotAssigned %d\n"), slot);

    hostedGame->clientData.selfIndex = slot;
}

void onDeplacementDemande(hosted_game* hostedGame, int direction)
{
    // si on a toujours pas recu le hello du serveur
    // on ne sait pas qui on est, alors on quitte tôt
    if(hostedGame->clientData.selfIndex == GAME_CLIENT_INDEX_UNASSIGNED) {
        return;
    }
    int selfIdx = hostedGame->clientData.selfIndex;

    struct vec2dir dir = {0};
    switch (direction) {
        case 'i':
            dir.y = -1; break;
        case 'k':
            dir.y = 1; break;
        case 'j':
            dir.x = -1; break;
        case 'l':
            dir.x = 1; break;
        default: break;
    }
    player *self = &(*hostedGame->board->players)[selfIdx];

    if(!canMoveAt(hostedGame->board, self, dir)) {
        return;
    }

    self->col += dir.x;
    self->line += dir.y;

    hostedGame->updateAsked = 1;
}

void onBombePoseDemande(hosted_game* hostedGame)
{
    // si on a toujours pas recu le hello du serveur
    // on ne sait pas qui on est, alors on quitte tôt
    if(hostedGame->clientData.selfIndex == GAME_CLIENT_INDEX_UNASSIGNED) {
        return;
    }
    int selfIdx = hostedGame->clientData.selfIndex;
    player *self = &(*hostedGame->board->players)[selfIdx];

    if(canLayBombAt(hostedGame->board, self)) {
        layBomb(hostedGame->board, self);
    }
}