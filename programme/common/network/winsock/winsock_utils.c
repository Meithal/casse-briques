//
// Created by hurin on 17/10/2022.
// Contient du code d'ici https://tangentsoft.net/wskfaq/examples/basics/index.html
//

#include "winsock_utils.h"
#include "stdio.h"
#include "tchar.h"
#include "client/cli/windows_compatibility/winterm.h"

int shutdownAsked = 0;
SOCKET client_sockets[0X10];

int StartWinsock()
{
    WSADATA WSAData = {0};
    if(WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
    {
        _tprintf(_T("Winsock fail to initialize error code %d"), WSAGetLastError());
        WSACleanup();
        return 1;
    }
    /* ... */

    _putts(_T("winsock demarre"));

    return 0;

}

int StopWinsock()
{
    if(WSACleanup() != 0)
    {
        _tprintf(_T("Winsock fail to cleanup error code %d"), WSAGetLastError());
        return 1;
    }

    _putts(_T("winsock stop"));

    return 0;
}

_TCHAR * FriendlyErrorMessage(int errorCode, DWORD bufferLen, _TCHAR *buffer) {

    if(0==FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            LANG_USER_DEFAULT, buffer, bufferLen, NULL
            )) {

        static _TCHAR error[256] = {0};
        _sntprintf(error, bufferLen, _T("Fail to generate error message for code %ld"), GetLastError());

        return error;
    }

    return buffer;
}

void StartServer(SOCKET * s, LPTHREAD_START_ROUTINE ThreadServeur, int serverPort)
{
    struct sockaddr_in server;
    int client_sockets_i = 0;

    //Create a socket
    if((*s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        _tprintf(_T("Could not create socket : %d") , WSAGetLastError());
        goto cleanup;
    }

    _tprintf(_T("Socket created.\n"));

    unsigned long ul = 1;
    int nRet = ioctlsocket(*s, FIONBIO, (unsigned long *) &ul);
    if(nRet == SOCKET_ERROR) {
        _TCHAR errBuffer[256];

        _tprintf(_T("Could not put socket in non blocking mode: %s") , FriendlyErrorMessage(
                WSAGetLastError(), 256, errBuffer)
                );
        goto cleanup;

    }




    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( serverPort );

    //Bind
    if( bind(*s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        _tprintf(_T("Bind failed with error code : %d") , WSAGetLastError());
        goto cleanup;
    }

    _putts(_T("Bind done."));

    listen(*s, SOMAXCONN);


    while (1) {
        struct sockaddr_in sinRemote;
        int sinsize = sizeof(sinRemote);
        _TCHAR errBuffer[256];
        SOCKET sd;
        if((sd = accept(*s, (struct sockaddr*)&sinRemote, &sinsize)) ==INVALID_SOCKET) {
            _tprintf(_T("Connéction invalide %"W"s\n"), FriendlyErrorMessage(
                    WSAGetLastError(), 256, errBuffer));
            goto cleanup;
        }

        printf("loop\n");

        _tprintf(_T("Connection depuis %s, %d"),
               inet_ntoa(sinRemote.sin_addr),
               ntohs(sinRemote.sin_port));

        DWORD nThreadId;

        CreateThread(0, 0, ThreadServeur, (void *) sd, 0, &nThreadId);
    }

cleanup:
    while (client_sockets_i--) {
        closesocket(client_sockets[client_sockets_i + 1]);
    }
    if (s != NULL)
        closesocket(*s);
    WSACleanup();

    return;
}

void CloseServer(const SOCKET *s)
{
    closesocket(*s);
    _putts(_T("Server closed."));
}



_Bool ShutdownConnection(SOCKET sd)
{
    // Disallow any further data sends.  This will tell the other side
    // that we want to go away now.  If we skip this step, we don't
    // shut the connection down nicely.
    if (shutdown(sd, SD_SEND) == SOCKET_ERROR) {
        return 0;
    }

    // Receive any extra data still sitting on the socket.  After all
    // data is received, this call will block until the remote host
    // acknowledges the TCP control packet sent by the shutdown above.
    // Then we'll get a 0 back from recv, signalling that the remote
    // host has closed its side of the connection.
    char acReadBuffer[K_BUFFER_SIZE];
    while (1) {
        int nNewBytes = recv(sd, acReadBuffer, K_BUFFER_SIZE, 0);
        if (nNewBytes == SOCKET_ERROR) {
            return 0;
        }
        else if (nNewBytes != 0) {
            _tprintf(_T("FYI, received %d unexpected bytes during shutdown.\n"), nNewBytes);
        }
        else {
            // Okay, we're done!
            break;
        }
    }

    // Close the socket.
    if (closesocket(sd) == SOCKET_ERROR) {
        return 0;
    }

    return 1;
}
