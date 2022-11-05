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


_Bool enableVtMode();
int setupConsoleForUnicode();

extern int charmap[0xff];
extern const int wide;

void loadCharmap();
#ifdef _UNICODE
#define W "l"
#else
#define W ""
#endif

#endif //CASSES_BRIQUES_WINTERM_H
