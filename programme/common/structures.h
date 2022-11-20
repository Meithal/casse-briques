//
// Created by hurin on 14/10/2022.
//

#ifndef CASES_BRIQUES_STRUCTURES_H
#define CASES_BRIQUES_STRUCTURES_H

#include "winsock2.h"

typedef struct field field;
struct field {
    wchar_t visual;
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
    int visual; // utilise pour le code client
    int position; // utilise pour le code client
    int max_bombes;
    int bombes_au_sol;
    int portee_bombe;
};

typedef struct bombe bombe;
struct bombe {
    int line;
    int col;
    unsigned long long laid_at_ms;
    unsigned long long fuse_time_ms;
};

typedef struct board board;
struct board {
    int rows;
    int cols;
    struct tile (* board)[];
    int nb_players;
    struct player (* players)[];
    int nb_bombes;
    struct bombe (* bombes)[];
};

typedef struct clientPlayer clientPlayer;
struct clientPlayer {
    struct player * player;
    SOCKET connection;
    struct sockaddr_in socketAddress;
};

typedef struct hosted_game hosted_game;
struct hosted_game {
    board *board;
    int mapNumber;
    int serverPort;
    struct {
        SOCKET serverSocket;
        int nbClients;
        clientPlayer (*clientPlayers)[];
    } hostData;
    struct {
        SOCKET clientSocket;
        int selfIndex;
        char * serverMessages;
        int serverMessagesCursor;
    } clientData;
    _Bool updateAsked;
};

extern field vide;
extern field bricks;
extern field mur;




#endif //CASES_BRIQUES_STRUCTURES_H
