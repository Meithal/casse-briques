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

    written += _sntprintf(
            buffer + written,
            size - written - 1,
            _T("player #: %d rows %d cols %d\n"),
            board->nb_players,
            board->rows,
            board->cols
    );

    for (int j = 0; j < board->rows; j++) {
        for (int i = 0; i < board->cols; ++i) {
            int vis = (*board->board)[j * board->cols + i].type->visual;
            vis = wide ? charmap[vis] : vis;
            player *playerAtPos = NULL;
            if(bombAt(board, j, i)) {
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
}