//
// Created by Utilisateur on 20/11/2022.
//

#include "tchar.h"

#include "callbacks.h"


void onGameSlotAssigned(hosted_game* hostedGame, int slot)
{
    _tprintf(_T("onGameSlotAssigned %d\n"), slot);
}