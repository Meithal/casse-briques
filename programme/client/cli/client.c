//
// Created by hurin on 05/11/2022.
//

#include "json.h"
#include "client.h"


static char *afficheJoueurs(int currentTile, struct player *player) {
    return "p";
}

void openNewConsole()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
                        NULL,        // Command line
                        NULL,           // Process handle not inheritable
                        NULL,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        CREATE_NEW_CONSOLE,              // No creation flags
                        NULL,           // Use parent's environment block
                        NULL,           // Use parent's starting directory
                        &si,            // Pointer to STARTUPINFO structure
                        &pi )           // Pointer to PROCESS_INFORMATION structure
            )
    {
        _tprintf( _T("CreateProcess failed (%d).\n"), GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

}

void sendRecv(SOCKET sock, int len, const char * buf, int outlen, char * outbuf)
{
    // Send an initial buffer
    int iResult = send( sock, buf, len, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sock);

        return;
    }

    _tprintf(_T("Bytes Sent: %ld\n"), iResult);

    // Receive until the peer closes the connection
    do {
        iResult = recv(sock, outbuf, outlen, 0);
        if ( iResult > 0 )
            _tprintf(_T("Bytes received: %d\n"), iResult);
        else if ( iResult == 0 )
            _tprintf(_T("Connection closed\n"));
        else
            _tprintf(_T("recv failed with error: %d\n"), WSAGetLastError());

    } while( iResult > 0 );

}
struct clientListener {
    SOCKET sock;
    char * outbuf;
};
DWORD WINAPI threadClientToServerListener(LPVOID pClientListener)
{
    struct clientListener * clientListener = pClientListener;

    int iResult;
    char recvbuf[0x200] = {0};
    do {
        iResult = recv(clientListener->sock, recvbuf, 0x200, 0);
        if ( iResult > 0 ) {
//            _tprintf(_T("Bytes received: %d\n"), iResult);
            strcpy(clientListener->outbuf, recvbuf);
        }
//        else if ( iResult == 0 )
//            _tprintf(_T("Connection closed\n"));
//        else
//            _tprintf(_T("recv failed with error: %d\n"), WSAGetLastError());

    } while( iResult > 0 );
}


DWORD WINAPI threadClient(LPVOID phosted_game) {

    hosted_game *hostedGame = phosted_game;
    _putts(_T("Bienvenue dans la partie en cours"));
    _putts(_T("Appuyez sur les flèches pour vous déplacer, q pour quitter."));

    _tprintf(_T("\033[2J"));

    //openNewConsole();
    char outBuf[0x200];
    wchar_t wOutBuf[0x200];

    SOCKET clientSock = startClient("localhost", hostedGame->serverPort);
    HANDLE listenerThread = CreateThread(
            NULL, 0,
            threadClientToServerListener, (LPVOID) &(struct clientListener) {
                    .sock = clientSock,
                    .outbuf = outBuf
            }, 0, NULL
    );

    while (1) {
        Sleep(200);
        _tprintf(_T("\033[H"));
//        _tprintf(_T("\033[2J"));

//        updateGameFromServer(hostedGame);

        _TCHAR bufOut[0x100] = {0};
        mapView(0x100, bufOut, hostedGame->board, afficheJoueurs);
        _putts(bufOut);

        if (kbhit()) {
            int ch = getch();

            if (ch == 'q') {
                break;
            }
            _tprintf(_T("%d\n"), ch);
        }
        _putts(_T("Messages du serveur"));
#ifdef _UNICODE
        mbstowcs(wOutBuf, outBuf, strlen(outBuf)+1);
        _tprintf(_T("%s\n"), wOutBuf);
#else
        _tprintf(_T("%10s\n"), outBuf);
#endif
    }

    closeClient(hostedGame->clientData.clientSocket);
    CloseHandle(listenerThread);

    _putts(_T(""));
    SetEvent(consoleWriteEvent);
    return 0;
}
