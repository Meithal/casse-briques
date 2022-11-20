//
// Created by hurin on 05/11/2022.
//

#include "grilleDisplay.h"

void mapView(int size,
             _TCHAR *buffer,
             board *board,
             char *(*displayCallback)(int currentTile, struct player *player)
){
    int written = 0;

//    _Bool (*map)[board->rows][board->cols] = calloc(board->cols*board->rows, sizeof (_Bool));
    _Bool map[board->rows][board->cols];
    for (int j = 0; j < board->rows; j++) {
        for (int i = 0; i < board->cols; ++i) {
            map[j][i] = 0;
        }
    }
    computeDeflagration(board->rows, board->cols, &map, board);

    written += _sntprintf(
            buffer + written,
            size - written - 1,
            _T("player #: %d rows %d cols %d\n"),
            board->nb_players,
            board->rows,
            board->cols
    );
    for (int i = 0; i < board->nb_players ; i++) {
        written += _sntprintf(
                buffer + written,
                size - written - 1,
                _T("player %d max bombes: %d bombes au sol %d\n"),
                i,
                (*board->players)[i].max_bombes,
                (*board->players)[i].bombes_au_sol
        );
    }
    for (int i = 0; i < board->nb_players * 5 ; i++) {
        bombe *bombe = &(*board->bombes)[i];
        if(bombe->col == -1) continue;

        written += _sntprintf(
                buffer + written,
                size - written - 1,
                _T("bombe %d posee at %lld porte %d doit exploser %d explose a %lld\n"),
                i,
                bombe->laid_at_ms,
                bombe->portee,
                shouldExplodeBomb(bombe),
                bombe->explosed_at_ms
        );
    }
    for (int j = 0; j < board->rows; j++) {
        for (int i = 0; i < board->cols; ++i) {
            if(isInDeflagration(board, board->rows, board->cols, &map, j, i)) {
                written += _sntprintf(buffer + written, size - written - 1, _T("*"));
            } else {
                written += _sntprintf(buffer + written, size - written - 1, _T("."));
            }
        }
        written += _sntprintf(buffer + written, size - written - 1, _T("\n"));
    }
    written += _sntprintf(buffer + written, size - written - 1, _T("\n"));

    for (int j = 0; j < board->rows; j++) {
        for (int i = 0; i < board->cols; ++i) {
            int vis = (*board->board)[j * board->cols + i].type->visual;
            vis = wide ? charmap[vis] : vis;
            player *playerAtPos = NULL;
            if(isInDeflagration(board, board->rows, board->cols, &map, j, i)) {
                written += _sntprintf(buffer + written, size - written - 1, _T("*"));
            } else if(bombAt(board, j, i)) {
                written += _sntprintf(buffer + written, size - written - 1, _T("*"));
            }
            else if (displayCallback != NULL && (playerAtPos = playerAtPosition(board, j, i))) {
                written += _sntprintf(
                        buffer + written, size - written - 1, _T("%"
                W
                "s"),
                displayCallback(vis, playerAtPos)
                );
            } else {
                written += _sntprintf(
                        buffer + written, size - written - 1, _T("%"
                W
                "c"),
                vis
                );
            }

        }
        written += _sntprintf(buffer + written, size - written - 1, _T("\n"));
    }

//    free(map);
}