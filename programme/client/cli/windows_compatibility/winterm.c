//
// Created by hurin on 16/10/2022.
//

#include "winterm.h"
#include "errno.h"

#include "common/structures.h"

int charmap[0xff] = {0};
#ifdef _UNICODE
const int wide = 1;
#else
const int wide = 0;
#endif

_Bool enableVtMode()
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

int setupConsoleForUnicode() {
    int result = -1;
#ifndef _UNICODE
    return result;
#endif
    // Set "stdin" to have wide text mode:
    result = _setmode( _fileno( stdout ), _O_WTEXT );
    if( result == -1 )
        _ftprintf(stderr, _T("Cannot set mode" ));
    else
        _tprintf( _T("'stdout' successfully changed to unicode mode\n" ));

    return result;
}

void loadCharmap()
{
    char line[256] = {0};

    _putts(_T("loading charmap"));
    fflush(stdout);

    FILE* f = fopen("assets/CP850.TXT", "r");

    if(f == NULL) {
        perror(NULL);
        fflush(stderr);
        return;
    }

    int val = 0;
    int pushed = 0;
    while (fgets(line, 256, f)) {
        if(line[0] != '0') {
            continue;
        }
        sscanf(line, "%*x\t%x", &val);
        charmap[pushed++] = val;
//        _tprintf(_T("%d\n"), val);
    }

    fclose(f);
}

