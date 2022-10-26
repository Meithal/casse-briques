//
// Created by Utilisateur on 26/10/2022.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "common/structures.h"
#include "gameRules.h"
#include "client/cli/windows_compatibility/winterm.h"

field vide = {
        .visual = 32,
        .txt = ' ',
        .destructible = 0,
};

field bricks = {
        .visual = 176,
        .txt = 'm',
        .destructible = 1,
};

field mur = {
        .visual = 219,
        .txt = 'x',
        .destructible = 0,
};


int loadMap(char* path, board * board)
{
    FILE* f = fopen(path, "r");

    if(f == NULL) {
        perror(NULL);
        fflush(stderr);

        return 1;
    }

    int bombes;
    int lignes;
    int cols;

    char line[256];
    fgets(line, 256, f);
    sscanf(line, "bombs=%d", &bombes);
    fgets(line, 256, f);
    sscanf(line, "column=%d", &cols);
    fgets(line, 256, f);
    sscanf(line, "row=%d", &lignes);

    board->board = malloc(sizeof (tile[lignes][cols]));
    board->players = NULL;

    int players = 0;
    char c;
    for (int i = 0 ; i < lignes * cols ; i++) {
        fscanf(f, " %c", &c);
        (*board->board)[i] = (struct tile) {
            .type = (c == 'p' || c == vide.txt) ? &vide : c == mur.txt ? &mur : &bricks,
            .destroyed = 0
        };

        if(c == 'p') {
            board->players = realloc(board->players, sizeof (struct player) * (players + 1));
            board->players[players].line = i / cols;
            board->players[players].col = i % cols;

            players++;
        }
    }

    board->nb_players = players;
    board->rows = lignes;
    board->cols = cols;

    fclose(f);

    return 0;
}

void unloadMap(board *board) {
    free(board->players);
    free(board->board);
}

void mapView(int size, _TCHAR *buffer, board *board) {
    int written = 0;
    written += _stprintf(
            buffer+written,
            size,
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
            written+=_stprintf(
                    buffer+written, size, _T("%lc"),
                      wide?charmap[(*board->board)->type->visual]:(*board->board)->type->visual
                      );
//            _tcscat(buffer, (const _TCHAR *) &(_TCHAR[2]) {
//                    wide ? charmap[board->board[j * i + i].type->visual] : board->board[j * i + i].type->visual,
//                    _T('\0')
//            });
        }
        written += _stprintf(buffer+written, size, _T("\n"));

    }
//    _tcscat(buffer, _T("\n"));

}
