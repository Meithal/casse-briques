//
// Created by hurin on 10/10/2022.
//

#include "stdio.h"

#include "conio.h"

#include "SDL.h"
#include "freetype/freetype.h"

#include "common/structures.h"

typedef struct freetype_wrapper {
    FT_Library ftLibrary;
    FT_Face  ftFaces[4];
    int numFaces;
} t_ftw;


int loadFreetype(t_ftw * ftw, float hdpi, float vdpi, int char_width) {
    FT_Library ftLibrary;
    FT_Face ftFace;

    FT_Error ftError;

    ftError = FT_Init_FreeType(&ftLibrary);

    if(ftError) {
        printf("Error loading ftlibrary %s", FT_Error_String(ftError));
        return -1;
    }

    ftError = FT_New_Face(ftLibrary, "assets/fonts/Montserrat-Medium.otf", 0, &ftFace);

    if(ftError) {
        printf("Error loading the face %s", FT_Error_String(ftError));
        FT_Done_FreeType(ftLibrary);
        return -1;
    }
    printf("Loaded font with %ld faces included, %ld glyphs, %hd units per EM, %d fixed sizes.\n",
            ftFace->num_faces, ftFace->num_glyphs, ftFace->units_per_EM, ftFace->num_fixed_sizes
            );

    ftError = FT_Set_Char_Size(
            ftFace, char_width*64, 0,
            (FT_UInt)hdpi, (FT_UInt)vdpi
            );

    if(ftError) {
        printf("Error setting the size %s", FT_Error_String(ftError));
        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLibrary);
        return -1;
    }

    puts("Ft lib loaded.");

    ftw->ftLibrary = ftLibrary;
    ftw->ftFaces[0] = ftFace;
    ftw->numFaces++;

    return 0;
}


void unloadFreeType(t_ftw * ftw) {
    for (int i = 0; i < ftw->numFaces ; i++) {
        FT_Done_Face(ftw->ftFaces[i]);
    }

    FT_Done_FreeType(ftw->ftLibrary);

    puts("Freetype unloaded.");
}

void writeFreetype(char* text, FT_Face face, int x, int y, SDL_Window * sdlWindow) {
    FT_GlyphSlot slot;
    FT_Vector pen;
    FT_Error ftError;

    slot = face->glyph;
    pen.x = x;
    pen.y = y;

    SDL_Color colors[256];
    for( int i = 0; i < 256; i++)
    {
        colors[i].r = colors[i].g = colors[i].b = i;
    }

//    SDL_RGB()

    for (int i = 0; text[i]; i++) {
        ftError = FT_Load_Char(face, text[i], FT_LOAD_RENDER);

        if(ftError) {
            printf("Error loading char %s", FT_Error_String(ftError));

            continue; // ignore errors
        }

        ftError = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
        if ( ftError ) {
            printf("Error rendering glyph %s", FT_Error_String(ftError));

            continue;
        }


        SDL_Surface * glyph = SDL_CreateRGBSurfaceFrom(
                slot->bitmap.buffer,
                slot->bitmap.width, slot->bitmap.rows,
                8, slot->bitmap.pitch, 0, 0, 0, 0xFF
                );

        // https://stackoverflow.com/a/50159550/13577241
        SDL_SetPaletteColors(glyph->format->palette, colors, 0, 256);
        SDL_SetSurfaceBlendMode(glyph, SDL_BLENDMODE_NONE);

        SDL_BlitSurface(
                glyph, NULL,
                SDL_GetWindowSurface(sdlWindow),
                &(SDL_Rect){pen.x, pen.y, 0,0}
                );
        SDL_FreeSurface(glyph);

        pen.x += slot->advance.x >> 6;
        pen.y += slot->advance.y >> 6;

    }
    SDL_RenderPresent(SDL_GetRenderer(sdlWindow));
    SDL_UpdateWindowSurface(sdlWindow);
}

int main(int argc, char** argv) {

    t_ftw freetypeWrapper = {0};

    // https://zestedesavoir.com/tutoriels/1014/utiliser-la-sdl-en-langage-c/dessiner-dans-la-fenetre/
    int init = SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Color orange = {255, 127, 40, 255};

    if(init < 0) {
        printf("error init %s \n", SDL_GetError());
        goto Quit;
    }

    SDL_Window * sdlWindow = SDL_CreateWindow(
            "Casse briques", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480, SDL_WINDOW_SHOWN);

    if(NULL == sdlWindow)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        goto Quit;
    }

//    SDL_Window * window2 = SDL_CreateWindow("SDL2", 700, 500,
//                               640, 480, SDL_WINDOW_SHOWN);
//    if(NULL == window2)
//    {
//        fprintf(stderr, "Erreur SDL_CreateWindow 2 : %s", SDL_GetError());
//        goto QuitWindow1;
//    }

    SDL_Renderer * sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == sdlRenderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        goto QuitWindow1;
    }

    float ddpi, hdpi, vdpi;
    SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
    printf("DPI ecran ddpi %f, hdpi %f, vdpi %f\n", ddpi, hdpi, vdpi);

    if(0!=loadFreetype(&freetypeWrapper, hdpi, vdpi, 140)) {
        goto QuitRenderer;
    }

    if(0!= SDL_SetRenderDrawColor(sdlRenderer, orange.r, orange.g, orange.b, orange.a)) {
        goto QuitRenderer;
    }

    if(0 != SDL_RenderClear(sdlRenderer))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s", SDL_GetError());
        goto QuitRenderer;
    }

    SDL_Delay(500);
    SDL_RenderPresent(sdlRenderer);

    SDL_Point point[640];
    SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255);

    for(int i = 0; i < 640; i++)
    {
        point[i].x = i* 5;
        point[i].y = 100;
    }
    SDL_RenderDrawPoints(sdlRenderer, point, 640);
    SDL_RenderPresent(sdlRenderer);

    SDL_Rect rect = {100, 100, 100, 100};
    SDL_RenderFillRect(sdlRenderer, &rect);
    SDL_RenderPresent(sdlRenderer);

    writeFreetype("toto", freetypeWrapper.ftFaces[0], 50, 50, sdlWindow);

    SDL_Delay(3000);

    unloadFreeType(&freetypeWrapper);
    QuitRenderer:SDL_DestroyRenderer(sdlRenderer);
//    QuitWindow2:SDL_DestroyWindow(window2);
    QuitWindow1:SDL_DestroyWindow(sdlWindow);
    Quit:SDL_Quit();

    return 0;
}