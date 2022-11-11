//
// Created by hurin on 05/11/2022.
//

#ifndef CASSES_BRIQUES_CLIENT_H
#define CASSES_BRIQUES_CLIENT_H

#include <winsock2.h>
#include <conio.h>
#include <tchar.h>

#include "common/structures.h"
#include "common/network/winsock/winsock_utils.h"
#include "client/cli/grilleDisplay.h"


DWORD WINAPI threadClient(LPVOID phosted_game);


#endif //CASSES_BRIQUES_CLIENT_H
