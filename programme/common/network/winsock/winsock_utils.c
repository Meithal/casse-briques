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
int client_sockets_i = 0;
HANDLE consoleWriteEvent;


int startWinsock()
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

int stopWinsock()
{
    if(WSACleanup() != 0)
    {
        _tprintf(_T("Winsock fail to cleanup error code %d"), WSAGetLastError());
        return 1;
    }

    _putts(_T("winsock stop"));

    return 0;
}

static DWORD bufferLen = 256;
static _TCHAR friendlyBuffer[256];
_TCHAR * friendlyErrorMessage(DWORD errorCode) {
    memset(friendlyBuffer, 0, sizeof friendlyBuffer);

    int written = _sntprintf(friendlyBuffer, bufferLen, _T("%ld : "), errorCode);
//    int written = 0;

    if(0==FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            LANG_USER_DEFAULT, friendlyBuffer + written, bufferLen - written, NULL
            )) {

        static _TCHAR error[256] = {0};
        _sntprintf(error, bufferLen, _T("Fail to generate error message for code %ld"), GetLastError());

        return error;
    }

    return friendlyBuffer;
}

void startServer(SOCKET *s, LPTHREAD_START_ROUTINE threadServerListenClient, int *serverPort, void *arguments)
{
    struct sockaddr_in server;
    WSAEVENT listenSocketEvent;

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

        _tprintf(_T("Could not put socket in non blocking mode: %"W"s") , friendlyErrorMessage(
                         WSAGetLastError())
                );
        goto cleanup;

    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    do {
        server.sin_port = htons(*serverPort);

        //Bind
        if (bind(*s, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
            if(WSAGetLastError() == 10048) {// protocol/adress/port already in use
                (*serverPort)++;
                _tprintf(_T("Port already used, trying with : %d\n"), *serverPort);

                continue;
            }
            _tprintf(_T("Bind failed with error code : %"W"s"), friendlyErrorMessage(WSAGetLastError()));
            goto cleanup;
        } else break;
    } while (1);

    _putts(_T("Bind done."));

    if(!SetEvent(consoleWriteEvent)) {
        _fputts( _T("Impossible de signaler un evenement!"), stderr);
    }

    //Create event
    listenSocketEvent = WSACreateEvent();

    WSAEventSelect(*s, listenSocketEvent, FD_ACCEPT | FD_CLOSE);

    //Lisen
    if(listen(*s, SOMAXCONN) == SOCKET_ERROR) {
        _tprintf(_T("Error setting : %d") , WSAGetLastError());
        goto cleanup;
    }

    while (1) {

        if(shutdownAsked) goto cleanup;

        struct sockaddr_in sinRemote;
        int sinsize = sizeof(sinRemote);
        SOCKET sd;
        if((sd = accept(*s, (struct sockaddr*)&sinRemote, &sinsize)) ==INVALID_SOCKET) {
            if(WSAGetLastError() == WSAEWOULDBLOCK) {
                continue;
            }
            _tprintf(_T("Connection invalide %"W"s\n"), friendlyErrorMessage(
                    WSAGetLastError()));
            goto cleanup;
        }

        TCHAR outBuf[40] = {0};
        InetNtop(AF_INET, &sinRemote.sin_addr, outBuf, 40);

        _tprintf(_T("Connection depuis %"W"s, notre port %d\n"),
               outBuf,
               ntohs(sinRemote.sin_port));

        struct sockaddr remote;
        socklen_t addrlen = sizeof remote;
        if(0!=getpeername(sd, &remote, &addrlen)) {
            _tprintf(_T("getpeername() fail %"W"s\n"), friendlyErrorMessage(
                    WSAGetLastError()));
        }
        _tprintf(_T("Port distant (serveur) %d\n"),
                 ntohs(((struct sockaddr_in*)&remote)->sin_port));


        DWORD nThreadId;

        CreateThread(
            0,
            0,
            threadServerListenClient,
            (void *) &(struct threadServerArguments) {
                .toClientSocket = sd, .socketAddress = sinRemote, .extras = arguments
            },
            0,
            &nThreadId
        );
    }

cleanup:return;
}

SOCKET startClient(char* address, int port)
{
    SOCKET connectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;
    int iResult;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    char servicePort[10] = {0};
    sprintf(servicePort, "%d", port);

    iResult = getaddrinfo(address, servicePort, &hints, &result);
    if ( iResult != 0 ) {
        _tprintf(_T("getaddrinfo failed with error: %d\n"), iResult);

        return 0;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            _tprintf(_T("socket failed with error: %ld\n"), WSAGetLastError());

            return 0;
        }

        // Connect to server.
        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }

        _tprintf(_T("Connected to remote, our port is %d\n"),
                 ntohs(((struct sockaddr_in*)ptr->ai_addr)->sin_port)
        );

        struct sockaddr remote;
        socklen_t addrlen = sizeof remote;
        if(0!=getpeername(connectSocket, &remote, &addrlen)) {
            _tprintf(_T("getpeername() fail %"W"s\n"), friendlyErrorMessage(
                    WSAGetLastError()));
        }

        _tprintf(_T("Port distant (client) %d\n"),
                 ntohs(((struct sockaddr_in*)&remote)->sin_port));

        break;
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) {
        _tprintf(_T("Unable to connect to server!\n"));
        return 0;
    }

    _tprintf(_T("Connected to server!\n"));
    return connectSocket;
}

void closeClient(const SOCKET s)
{
    // shutdown the connection since no more data will be sent
    int iResult = shutdown(s, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        _tprintf(_T("shutdown failed with error: %d\n"), WSAGetLastError());
        closesocket(s);

        return;
    }

    // cleanup
    closesocket(s);

}


void closeServer(const SOCKET *s)
{
    while (client_sockets_i--) {
        closesocket(client_sockets[client_sockets_i + 1]);
    }
    if (s != NULL)
        closesocket(*s);

    _putts(_T("Server closed."));
}



_Bool shutdownConnection(SOCKET sd)
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

_Bool connectionClient(const SOCKET sd, void(*callback)(void* extra, char* message), void* extraa) {
    // Read data from client
    char acReadBuffer[K_BUFFER_SIZE];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, K_BUFFER_SIZE, 0);
        if (nReadBytes > 0) {
            _tprintf(_T("Received %d bytes from client.\n"), nReadBytes);
            _putts(_T("\a"));

            callback(extraa, acReadBuffer);
        }
        else if (nReadBytes == SOCKET_ERROR) {
            if(WSAGetLastError() == WSAEWOULDBLOCK) {
                continue;
            }
            return 0;
        }
    } while (nReadBytes != 0);

    _putts(_T("Connection closed by peer."));
    return 1;
}
