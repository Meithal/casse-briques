//
// Created by Utilisateur on 30/10/2022.
//

#ifndef CASSES_BRIQUES_CONIO_POLYFILL_H
#define CASSES_BRIQUES_CONIO_POLYFILL_H

#include "stdlib.h"
#include "unistd.h"
#include "sys/socket.h"

int kbhit();
ssize_t getch();

#endif //CASSES_BRIQUES_CONIO_POLYFILL_H
