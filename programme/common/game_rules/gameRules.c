//
// Created by Utilisateur on 26/10/2022.
//

#include "stdio.h"
#include "stdlib.h"

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

    #ifdef _MSC_VER
    board->board = malloc(sizeof (tile)* lignes*cols);
    #else
    board->board = malloc(sizeof (tile[lignes][cols]));
    #endif
    board->players = NULL;

//    typedef tile (boardptr)[board->rows][board->cols];

    int players = 0;
    char c;
    for (int j = 0 ; j < lignes ; j++) {
        for (int i = 0; i < cols; i++) {
            fscanf(f, " %c", &c);
            (*board->board)[j][i] = (struct tile) {
                    .type = (c == 'p' || c == vide.txt) ? &vide : c == mur.txt ? &mur : &bricks,
                    .destroyed = 0
            };

//        if(c == 'p') {
//            void * newPlayers = realloc(board->players, sizeof (struct player) * (players + 1));
//            if(newPlayers == NULL) {
//                unloadMap(board);
//                return 1;
//            }
//            board->players = newPlayers;
//            (*board->players)[players].line = j;
//            (*board->players)[players].col = i;
//
//            players++;
//        }
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
            written+=_stprintf(
                    buffer+written, size - written - 1, _T("%lc"),
                      wide?charmap[(*board->board)[j][i].type->visual]:(*board->board)[j][i].type->visual
                      );
//            _tcscat(buffer, (const _TCHAR *) &(_TCHAR[2]) {
//                    wide ? charmap[board->board[j * i + i].type->visual] : board->board[j * i + i].type->visual,
//                    _T('\0')
//            });
        }
        written += _stprintf(buffer+written, size - written - 1, _T("\n"));

    }
//    _tcscat(buffer, _T("\n"));

}
