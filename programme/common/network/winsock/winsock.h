//
// Created by hurin on 17/10/2022.
//

#ifndef CASSES_BRIQUES_WINSOCK_H
#define CASSES_BRIQUES_WINSOCK_H

#include "winsock2.h"

int StartWinsock();
int StopWinsock();
void StartServer(SOCKET * s, DWORD WINAPI (ThreadServeur)(void* sd_));
void CloseServer(const SOCKET *s);
_Bool ShutdownConnection(SOCKET sd);

#endif //CASSES_BRIQUES_WINSOCK_H
