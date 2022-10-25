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

typedef struct board board;
struct board {
    struct field *** board;
};


#endif //CASES_BRIQUES_STRUCTURES_H
