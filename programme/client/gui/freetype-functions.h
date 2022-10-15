//
// Created by hurin on 15/10/2022.
//

#ifndef CASSES_BRIQUES_FREETYPE_FUNCTIONS_H
#define CASSES_BRIQUES_FREETYPE_FUNCTIONS_H

#include "freetype/freetype.h"
#include "SDL.h"

typedef struct freetype_wrapper {
    FT_Library ftLibrary;
    FT_Face  ftFaces[4];
    int numFaces;
} t_ftw;

int loadFreetype(t_ftw * ftw, float hdpi, float vdpi, int char_width);
void unloadFreeType(t_ftw * ftw);
void writeFreetype(char* text, FT_Face face, int x, int y, SDL_Window * sdlWindow);

#endif //CASSES_BRIQUES_FREETYPE_FUNCTIONS_H
