//
// Created by hurin on 14/10/2022.
//

#ifndef CASES_BRIQUES_STRUCTURES_H
#define CASES_BRIQUES_STRUCTURES_H

typedef struct field field;
struct field {
    wchar_t visual;
    char txt;
    int destructible;
};

typedef struct player player;
struct player {
    int visual;
    int position;
};

#endif //CASES_BRIQUES_STRUCTURES_H
