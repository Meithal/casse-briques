//
// Created by hurin on 16/10/2022.
//

#include "winterm.h"

_Bool EnableVTMode()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return 0;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return 0;
    }
    return 1;
}

int SetupConsoleForUnicode() {
    int result = 0;
#ifndef _UNICODE
    return result;
#endif
    // Set "stdin" to have wide text mode:
    result = _setmode( _fileno( stdout ), _O_WTEXT );
    if( result == -1 )
        perror( "Cannot set mode" );
    else
        printf( "'stdin' successfully changed to binary mode\n" );

    return result;
}