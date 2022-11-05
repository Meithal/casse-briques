//
// Created by hurin on 14/10/2022.
//

#ifndef CASES_BRIQUES_STRUCTURES_H
#define CASES_BRIQUES_STRUCTURES_H

#include "winsock2.h"

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
    int is_ia;
};

typedef struct board board;
struct board {
    int rows;
    int cols;
    struct tile (* board)[];
    int nb_players;
    struct player (* players)[];
};

typedef struct clientPlayer clientPlayer;
struct clientPlayer {
    struct player * player;
    SOCKET connection;
};

typedef struct hosted_game hosted_game;
struct hosted_game {
    board *board;
    int mapNumber;
    int serverPort;
    SOCKET serverSocket;
    int nbClients;
    clientPlayer clientPlayers[];
};

extern field vide;
extern field bricks;
extern field mur;




#endif //CASES_BRIQUES_STRUCTURES_H
