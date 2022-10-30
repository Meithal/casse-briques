//
// Created by hurin on 17/10/2022.
//

#ifndef CASSES_BRIQUES_WINSOCK_UTILS_H
#define CASSES_BRIQUES_WINSOCK_UTILS_H

#include "winsock2.h"
#include "windows.h"

#define K_BUFFER_SIZE 1024

int StartWinsock();
int StopWinsock();
void StartServer(SOCKET * s, LPTHREAD_START_ROUTINE (ThreadServeur), int serverPort);
void CloseServer(const SOCKET *s);
_Bool ShutdownConnection(SOCKET sd);

#endif //CASSES_BRIQUES_WINSOCK_UTILS_H
