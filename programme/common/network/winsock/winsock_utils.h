//
// Created by hurin on 17/10/2022.
//

#ifndef CASSES_BRIQUES_WINSOCK_UTILS_H
#define CASSES_BRIQUES_WINSOCK_UTILS_H
#define WIN32_LEAN_AND_MEAN

#include "winsock2.h"
#include <ws2tcpip.h>

#include "windows.h"
#include "tchar.h"

#define K_BUFFER_SIZE 1024

struct threadServerArguments {
    SOCKET serverSocket;
    struct sockaddr_in socketAddress;
    void* extras;
};


int startWinsock();
int stopWinsock();
void startServer(SOCKET *s, LPTHREAD_START_ROUTINE threadServerListenClient, int *serverPort, void *arguments);
void closeServer(const SOCKET *s);
_Bool shutdownConnection(SOCKET sd);
_Bool connectionClient(SOCKET sd, void(*callback)(void* extra, char* message), void* extra);
_TCHAR * friendlyErrorMessage(DWORD errorCode);
SOCKET startClient(char* address, int port);
void closeClient(SOCKET s);

extern int shutdownAsked;
extern HANDLE consoleWriteEvent;


#endif //CASSES_BRIQUES_WINSOCK_UTILS_H
