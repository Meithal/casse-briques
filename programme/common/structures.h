//
// Created by hurin on 14/10/2022.
//

#ifndef CASES_BRIQUES_STRUCTURES_H
#define CASES_BRIQUES_STRUCTURES_H

typedef struct field field;
struct field {
    int visual;
    char txt;
    int destructible;
};

typedef struct tile tile;
struct tile {
    struct field * type;
    int destroyed;
};

typedef struct player player;
struct player {
    int line;
    int col;
};

typedef struct board board;
struct board {
    int rows;
    int cols;
    struct tile (* board)[][32];
    int nb_players;
    struct player (* players)[];
};

extern field vide;
extern field bricks;
extern field mur;




#endif //CASES_BRIQUES_STRUCTURES_H
