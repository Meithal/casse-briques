//
// Created by Utilisateur on 30/10/2022.
//

#include "askIntInput.h"
#include "tchar.h"

int askIntInput(int lowIncluded, int highIncluded)
{
    long val;
    do {
        _tprintf(">");
        char buffer[5] = {0};

        int written = fscanf(stdin, "%4s", buffer);

        char * end;
        val = strtol(buffer, &end, 10);

        if(end - buffer == written && val >= lowIncluded && val <= highIncluded) {
            break;
        }
        puts("Nombre invalide");
    }while (1);

    return val;
}
