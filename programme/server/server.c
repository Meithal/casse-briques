//
// Created by hurin on 17/10/2022.
//

#include "stdio.h"

#include <winsock2.h>
#include <stdbool.h>

#include "common/network/winsock/winsock.h"

#include "server.h"

bool ConnectionClient(SOCKET sd);
DWORD WINAPI ThreadServeur(void* sd_);

int main()
{
//    WSADATA WSAData;
//    SOCKET sock;
//    SOCKET csock;
//    SOCKADDR_IN sin;
//    SOCKADDR_IN csin;
//    WSAStartup(MAKEWORD(2,0), &WSAData);
//    sock = socket(AF_INET, SOCK_STREAM, 0);
//    sin.sin_addr.s_addr = INADDR_ANY;
//    sin.sin_family = AF_INET;
//    sin.sin_port = htons(23);
//    bind(sock, (SOCKADDR *)&sin, sizeof(sin));
//    listen(sock, 0);
//    while(1) /* Boucle infinie. Exercice : améliorez ce code. */
//    {
//        int sinsize = sizeof(csin);
//        if((csock = accept(sock, (SOCKADDR *)&csin, &sinsize)) != INVALID_SOCKET)
//        {
//            send(csock, "Hello world!\r\n", 14, 0);
//            closesocket(csock);
//        }
//    }
    /* On devrait faire closesocket(sock); puis WSACleanup(); mais puisqu'on a entré une boucle infinie ... */

    StartWinsock();
    SOCKET s;
    StartServer(&s, ThreadServeur);
    CloseServer(&s);
    StopWinsock();

    return 0;
}

DWORD WINAPI ThreadServeur(void* sd_) {
    int nRetval = 0;
    SOCKET sd = (SOCKET)sd_;

    if (!ConnectionClient(sd)) {
        printf("Erreur avec le client\n");
        nRetval = 3;
    }

    puts("Fermeture connection avec client...");
    if (ShutdownConnection(sd)) {
        printf("Connection is down.");
    }
    else {
        fprintf(stderr, "Connection shutdown failed\n");
        nRetval = 3;
    }

    return nRetval;
}

bool ConnectionClient(SOCKET sd) {
    // Read data from client
    int kBufferSize = 1024;
    char acReadBuffer[kBufferSize];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
        if (nReadBytes > 0) {
            printf("Received %d bytes from client.\n", nReadBytes);

            int nSentBytes = 0;
            while (nSentBytes < nReadBytes) {
                int nTemp = send(sd, acReadBuffer + nSentBytes,
                                 nReadBytes - nSentBytes, 0);
                if (nTemp > 0) {
                    printf("Sent %d bytes back to client.\n", nTemp);
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR) {
                    return false;
                }
                else {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    puts("Peer unexpectedly dropped connection!");
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR) {
            return false;
        }
    } while (nReadBytes != 0);

    puts("Connection closed by peer.");
    return true;
}
