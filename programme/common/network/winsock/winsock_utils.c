//
// Created by hurin on 17/10/2022.
// Contient du code d'ici https://tangentsoft.net/wskfaq/examples/basics/index.html
//

#include "winsock_utils.h"
#include "stdio.h"
#include "tchar.h"


int StartWinsock()
{
    WSADATA WSAData;
    if(WSAStartup(MAKEWORD(2,0), &WSAData) != 0)
    {
        _tprintf(_T("Winsock fail to initialize error code %d"), WSAGetLastError());
        return 1;
    };
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

TCHAR * FriendlyErrorMessage(int errorCode, DWORD bufferLen, LPSTR buffer) {

    if(0==FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            LANG_USER_DEFAULT, buffer, bufferLen, NULL
            )) {

        static TCHAR error[256] = {0};
        _stprintf(error, bufferLen, _T("Fail to generate error message for code %ld"), GetLastError());

        return error;
    }

    return buffer;
}

void StartServer(SOCKET * s, LPTHREAD_START_ROUTINE ThreadServeur)
{
    struct sockaddr_in server;

    //Create a socket
    if((*s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        _tprintf(_T("Could not create socket : %d") , WSAGetLastError());
    }

    _tprintf(_T("Socket created.\n"));


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 41480 );

    //Bind
    if( bind(*s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        _tprintf(_T("Bind failed with error code : %d") , WSAGetLastError());
    }

    _putts(_T("Bind done."));

    listen(*s, SOMAXCONN);

    while (1) {
        struct sockaddr_in sinRemote;
        int sinsize = sizeof(sinRemote);
        TCHAR errBuffer[256];
        SOCKET sd;
        if((sd = accept(*s, (struct sockaddr*)&sinRemote, &sinsize)) ==INVALID_SOCKET) {
            _tprintf(_T("Connéction invalide %s\n"), FriendlyErrorMessage(WSAGetLastError(), 256, errBuffer));
            return;
        }

        _tprintf(_T("Connection depuis %s, %d"),
               inet_ntoa(sinRemote.sin_addr),
               ntohs(sinRemote.sin_port));

        DWORD nThreadId;

        CreateThread(0, 0, ThreadServeur, (void *) sd, 0, &nThreadId);
    }
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
