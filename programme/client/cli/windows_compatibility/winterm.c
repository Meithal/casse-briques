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

void LoadCharmap()
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

void mapView(int size, _TCHAR *buffer, board *board) {
    int written = 0;
//    extern int charmap[0xff];
//    extern const int wide;

    written += _sntprintf(
            buffer+written,
            size - written - 1,
            _T("player #: %d rows %d cols %d\n"),
            board->nb_players,
            board->rows,
            board->cols
    );
//    _TCHAR buf[5] = {0};
//    _stprintf(buf, 5, _T("%d"), board->nb_players);
//    _tcscat(buffer, buf);
//    _tcscat(buffer, _T("\n"));

    for (int j = 0; j < board->rows ; j++) {
        for (int i = 0; i < board->cols; ++i) {
            int vis = (*board->board)[j*board->cols + i].type->visual;
            written+=_sntprintf(
                    buffer+written, size - written - 1, _T("%"W"c"),
                    wide?charmap[vis]:vis
            );
//            _tcscat(buffer, (const _TCHAR *) &(_TCHAR[2]) {
//                    wide ? charmap[board->board[j * i + i].type->visual] : board->board[j * i + i].type->visual,
//                    _T('\0')
//            });
        }
        written += _sntprintf(buffer+written, size - written - 1, _T("\n"));

    }
//    _tcscat(buffer, _T("\n"));

}
