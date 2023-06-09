//
// Created by Utilisateur on 26/10/2022.
//

#include "stdio.h"
#include "stdlib.h"

#include "common/structures.h"
#include "gameRules.h"


//extern const int wide;
//extern int charmap[0xff];


struct field vide = {
        .visual = 32,
        .txt = ' ',
        .destructible = 0,
};

struct field bricks = {
        .visual = 176,
        .txt = 'm',
        .destructible = 1,
};

struct field mur = {
        .visual = 219,
        .txt = 'x',
        .destructible = 0,
};


static struct tile getTileAt(struct board*board, int y, int x)
{
    return (*board->board)[board->cols * y + x];
}

int loadMap(char* path, struct board * board)
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
    board->board = malloc(sizeof (struct tile[lignes][cols]));
    #endif
    if (board->board == NULL) {
        return 1;
    }
    
    board->players = NULL;

//    typedef tile (boardptr)[board->rows][board->cols];

    int players = 0;
    char c;
    for (int j = 0 ; j < lignes ; j++) {
        for (int i = 0; i < cols; i++) {
            fscanf(f, "%c", &c);
            (*board->board)[j*cols+i] = (struct tile) {
                    .type = (c == 'p' || c == vide.txt) ? &vide : c == mur.txt ? &mur : &bricks,
                    .destroyed = 0
            };

        if(c == 'p') {
            void * newPlayers = realloc(board->players, sizeof (struct player) * (players + 1));
            if(newPlayers == NULL) {
                unloadMap(board);
                return 1;
            }
            board->players = newPlayers;
            (*board->players)[players].line = j;
            (*board->players)[players].col = i;
            (*board->players)[players].is_ia = 0;
            (*board->players)[players].is_dead = 0;
            (*board->players)[players].max_bombes = bombes;
            (*board->players)[players].bombes_au_sol = 0;
            (*board->players)[players].portee_bombe = 2;

            players++;
        }
        }
        fgetc(f);
    }

    board->nb_players = players;
    board->rows = lignes;
    board->cols = cols;

    board->bombes = calloc(players * 5, sizeof (struct bombe)); // arbitrairement 5 bombes en même temps par joueur
    if (board->bombes == NULL) {
        return 1;
    }
    for(int i = 0; i < players * 5; i++) {
        (*board->bombes)[i].col = -1;
        (*board->bombes)[i].line = -1;
        (*board->bombes)[i].fuse_time_ms = 5000; // 5 secondes par defaut
    }

//    fclose(f);

    return 0;
}

void unloadMap(struct board *board) {
    free(board->players);
    free(board->board);
    free(board->bombes);
}

int setAIPlayers(int number, struct board * board)
{
    if(number > board->nb_players) {
        return 0;
    }
    while(number--) {
        (*board->players)[number].is_ia = 1;
    }

    return 1;
}

int getPlacesRestantes(struct hosted_game * hostedGame)
{
    int restant = 0;
    struct board * board = hostedGame->board;
    int max = board->nb_players;

    restant = max;

    for(int i = 0; i < max ; i++) {
        struct player player = (*board->players)[i];
        if(player.is_ia) {
            restant--;
        }
    }

    restant -= hostedGame->hostData.nbClients;

    return restant;
}

struct hosted_game *hostedGameFromIdx(
        int hostedGamesMax,
        struct hosted_game games[], int idx)
{
    int found = 1;
    for(int i = 0; i < hostedGamesMax ; i++, found++) {
        if(!getPlacesRestantes(&games[i])) {
            continue;
        }
        if(found == idx) {
            return &games[i];
        }
    }

    return NULL;
}

struct player * playerAtPosition(struct board*board, int y, int x)
{
    for(int i = 0 ; i < board->nb_players ; i++)
    {
        struct player player = (*board->players)[i];
        if(player.col == x && player.line == y) {
            return &(*board->players)[i];
        }
    }
    return NULL;
}

_Bool canMoveAt(struct board*board,
        struct player*player, struct vec2dir to)
{
    struct tile tile = getTileAt(
            board,
            player->line + to.y, player->col + to.x);
    if(tile.type->txt == 'm' || tile.type->txt == 'x') {
        return 0;
    }

    if(NULL != bombAt(board, player->line + to.y, player->col + to.x)) {
        return 0;
    }

    return 1;
}

struct bombe * bombAt(struct board*board, int y, int x)
{
    for(int i = 0; i < board->nb_players * 5; i++) {
        struct bombe *bombe = &(*board->bombes)[i];
        // si une bombe existe deja a l'emplacement du joueur
        if(bombe->line == y && bombe->col == x) {
            return bombe;
        }
    }

    return NULL;
}

_Bool canLayBombAt(struct board*board, struct player*player)
{
    for(int i = 0; i < board->nb_players * 5; i++) {
        struct bombe bombe = (*board->bombes)[i];
        // si une bombe existe deja a l'emplacement du joueur
        if(bombe.line == player->line && bombe.col == player->col) {
            return 0;
        }
    }

    if(player->bombes_au_sol >= player->max_bombes) {
        return 0;
    }

    return 1;
}

void layBomb(struct board*board, struct player*player)
{
    player->bombes_au_sol ++;

    for(int i = 0; i < board->nb_players * 5; i++) {
        struct bombe *bombe = &(*board->bombes)[i];
        // si une bombe existe deja a l'emplacement du joueur
        if(bombe->line == -1 && bombe->col == -1) {
            bombe->owner = player;
            bombe->line = player->line;
            bombe->col = player->col;
            bombe->portee = player->portee_bombe;
            bombe->laid_at_ms = GetTickCount64();
            bombe->fuse_time_ms = 5000;
            return;
        }
    }
}

static void makeBombExplode(struct board*board, struct bombe* bombe) {
    bombe->owner->bombes_au_sol--;
    bombe->explosed_at_ms = GetTickCount64();
}

_Bool shouldExplodeBomb(struct bombe*bomb)
{
    ULONGLONG now = GetTickCount64();

    return bomb->laid_at_ms + bomb->fuse_time_ms < now;
}

void updateGameState(struct board*board)
{
    _Bool map[board->rows][board->cols];
    for (int j = 0; j < board->rows; j++) {
        for (int i = 0; i < board->cols; ++i) {
            map[j][i] = 0;
        }
    }
    computeDeflagration(board->rows, board->cols, &map, board);



    ULONGLONG now = GetTickCount64();

    repeat:
    for(int i = 0; i < board->nb_players * 5; i++) {
        struct bombe *bombe = &(*board->bombes)[i];
        if(bombe->laid_at_ms == 0) {
            continue;
        }
        if(bombe->explosed_at_ms != 0) { // une bombe qu'on a fait exploser on ne la refait pas exploser
            continue;
        }
        if(bombe->laid_at_ms + bombe->fuse_time_ms < now) {
            makeBombExplode(board, bombe);
            goto repeat;
        }
        if(isInDeflagration(board, board->rows, board->cols, &map, bombe->line, bombe->col)) {
            makeBombExplode(board, bombe);
        }
    }
    for(int i = 0; i < board->nb_players; i++) {
        struct player *player = &(*board->players)[i];

        if(isInDeflagration(board, board->rows, board->cols, &map, player->line, player->col)) {
            player->is_dead = 1;
        }
    }
}

_Bool isInDeflagration(
        struct board*board,
        int rows, int cols,
        _Bool (*map)[rows][cols], int y, int x) {

    return (*map)[y][x];
}

void computeDeflagration(
        int rows,
        int cols,
        _Bool (*map)[rows][cols],
        struct board*board)
{
    for(int i = 0; i < board->nb_players * 5; i++) {
        struct bombe *bombe = &(*board->bombes)[i];
        if(bombe->col == -1) continue;
        if(bombe->explosed_at_ms == 0) continue;
        for(int dy = -1; dy <= 1 ; dy += 2) {
            for(int portee = 0; portee <= bombe->portee ; portee ++) {
                int row_at = bombe->line + portee * dy;
                if(row_at < 0 || row_at >= board->rows) break;
                struct tile target = getTileAt(board, bombe->line + portee * dy, bombe->col);
                if(target.type->txt == 'x') break;
                if(target.type->txt == 'm'){
                    (*map)[bombe->line + portee * dy][bombe->col] = 1;
                    break;
                }
                (*map)[bombe->line + portee * dy][bombe->col] = 1;
            }
        }
        for(int dx = -1; dx <= 1; dx += 2) {
            for(int portee = 0; portee <= bombe->portee ; portee ++) {
                int col_at = bombe->col + portee * dx;
                if(col_at < 0 || col_at >= board->cols) break;
                struct tile target = getTileAt(board, bombe->line, bombe->col + portee * dx);
                if(target.type->txt == 'x') break;
                if(target.type->txt == 'm'){
                    (*map)[bombe->line][bombe->col + portee * dx] = 1;
                    break;
                }

                (*map)[bombe->line][bombe->col + portee * dx] = 1;
            }
        }
    }

}
