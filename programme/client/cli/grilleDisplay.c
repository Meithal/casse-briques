//
// Created by hurin on 05/11/2022.
//

#include "grilleDisplay.h"

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
