//
// Created by hurin on 17/10/2022.
//

#include "stdio.h"

#include <winsock2.h>
#include <stdbool.h>

#include "common/network/winsock/winsock_utils.h"


#ifdef _WIN32
#include "client/cli/windows_compatibility/winterm.h"
#endif

#include "server.h"
#include "common/game_rules/gameRules.h"
extern const int wide;
extern int charmap[0xff];

bool ConnectionClient(SOCKET sd);

DWORD WINAPI ThreadServeur(LPVOID sd_) {
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

#ifdef _WIN32
    SetupConsoleForUnicode();
    LoadCharmap();
#endif

    board board;
    loadMap("assets/maps/grille1.txt", &board);
    _TCHAR buf[0x100] = {0};
    mapView(0x100, buf, &board);
    _putts(buf);
    fflush(stdout);

    StartWinsock();
    SOCKET s;
    StartServer(&s, (LPTHREAD_START_ROUTINE) ThreadServeur);
    CloseServer(&s);
    StopWinsock();

    return 0;
}


bool ConnectionClient(SOCKET sd) {
    // Read data from client
    char acReadBuffer[K_BUFFER_SIZE];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, K_BUFFER_SIZE, 0);
        if (nReadBytes > 0) {
            _tprintf(_T("Received %d bytes from client.\n"), nReadBytes);

            int nSentBytes = 0;
            while (nSentBytes < nReadBytes) {
                int nTemp = send(sd, acReadBuffer + nSentBytes,
                                 nReadBytes - nSentBytes, 0);
                if (nTemp > 0) {
                    _tprintf(_T("Sent %d bytes back to client.\n"), nTemp);
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR) {
                    return false;
                }
                else {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    _putts(_T("Peer unexpectedly dropped connection!"));
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR) {
            return false;
        }
    } while (nReadBytes != 0);

    _putts(_T("Connection closed by peer."));
    return true;
}
