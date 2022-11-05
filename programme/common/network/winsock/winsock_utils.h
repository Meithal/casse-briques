//
// Created by hurin on 17/10/2022.
//

#ifndef CASSES_BRIQUES_WINSOCK_UTILS_H
#define CASSES_BRIQUES_WINSOCK_UTILS_H

#include "winsock2.h"
#include "windows.h"
#include "tchar.h"

#define K_BUFFER_SIZE 1024

int startWinsock();
int stopWinsock();
void startServer(SOCKET * s, LPTHREAD_START_ROUTINE (ThreadClient), int serverPort);
void closeServer(const SOCKET *s);
_Bool shutdownConnection(SOCKET sd);
_TCHAR * friendlyErrorMessage(DWORD errorCode);

extern int shutdownAsked;
extern HANDLE consoleWriteEvent;


#endif //CASSES_BRIQUES_WINSOCK_UTILS_H
