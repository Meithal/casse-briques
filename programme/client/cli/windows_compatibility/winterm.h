//
// Created by hurin on 16/10/2022.
//

#ifndef CASSES_BRIQUES_WINTERM_H
#define CASSES_BRIQUES_WINTERM_H

#include "windows.h"
#include <stdio.h>
#include <fcntl.h>
#include "io.h"
#include <tchar.h>


_Bool EnableVTMode();
int SetupConsoleForUnicode();

#endif //CASSES_BRIQUES_WINTERM_H
