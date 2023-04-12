//
// Created by hurin on 14/10/2022.
//

#ifndef CASES_BRIQUES_STRUCTURES_H
#define CASES_BRIQUES_STRUCTURES_H

#include "winsock2.h"

struct field {
    wchar_t visual;
    char txt;
    int destructible;
};

struct tile {
    struct field * type;
    int destroyed;
};

struct player {
    int line;
    int col;
    int is_ia;
    int visual; // utilise pour le code client
    int position; // utilise pour le code client
    int max_bombes;
    int bombes_au_sol;
    int portee_bombe;
    int is_dead;
};

struct bombe {
    int line;
    int col;
    int portee;
    unsigned long long laid_at_ms;
    unsigned long long fuse_time_ms;
    unsigned long long explosed_at_ms;
    struct player * owner;
};

struct board {
    int rows;
    int cols;
    struct tile (* board)[];
    int nb_players;
    struct player (* players)[];
    int nb_bombes;
    struct bombe (* bombes)[];
};

struct clientPlayer {
    struct player * player;
    SOCKET connection;
    struct sockaddr_in socketAddress;
};

struct hosted_game {
    struct board *board;
    int mapNumber;
    int serverPort;
    struct {
        SOCKET serverSocket;
        int nbClients;
        struct clientPlayer (*clientPlayers)[];
    } hostData;
    struct {
        SOCKET clientSocket;
        int selfIndex;
        char * serverMessages;
        int serverMessagesCursor;
    } clientData;
    _Bool updateAsked;
};

extern struct field vide;
extern struct field bricks;
extern struct field mur;




#endif //CASES_BRIQUES_STRUCTURES_H
