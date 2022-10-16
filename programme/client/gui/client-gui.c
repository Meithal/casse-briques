//
// Created by hurin on 10/10/2022.
//

#include "stdio.h"

#include "conio.h"
#include "SDL.h"

#include "common/structures.h"
#include "freetype-functions.h"

//SDL_Window * createSDL

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

    SDL_Window * window2 = SDL_CreateWindow("SDL2", 700, 500,
                               640, 480, SDL_WINDOW_SHOWN);
    if(NULL == window2)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow 2 : %s", SDL_GetError());
        goto Quit;
    }

    SDL_Renderer * sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == sdlRenderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        goto Quit;
    }

    float ddpi, hdpi, vdpi;
    SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
    printf("DPI ecran ddpi %f, hdpi %f, vdpi %f\n", ddpi, hdpi, vdpi);

    if(0!=loadFreetype(&freetypeWrapper, hdpi, vdpi, 40)) {
        goto Quit;
    }

    if(0!= SDL_SetRenderDrawColor(sdlRenderer, orange.r, orange.g, orange.b, orange.a)) {
        goto Quit;
    }

    if(0 != SDL_RenderClear(sdlRenderer))
    {
        fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s", SDL_GetError());
        goto Quit;
    }

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

    SDL_Delay(500);

    writeFreetype("casses briques", freetypeWrapper.ftFaces[0], 50, 50, sdlWindow);

    SDL_bool continuer = SDL_TRUE;

    SDL_Event event;
    while (continuer) {
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT) {
            continuer = SDL_FALSE;
        }
        if(event.type == SDL_WINDOWEVENT) {
            printf("window event %d\n", event.window.event);
            if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
                printf("window event close %d\n", event.window.windowID);
                SDL_Window * win = SDL_GetWindowFromID(event.window.windowID);
                SDL_DestroyWindow(win);
                win = NULL;
            } else if (event.window.type == SDL_WINDOWEVENT_RESIZED) {
                puts("resize");
            }
        }
        SDL_Delay(20);
    }

    Quit:
    unloadFreeType(&freetypeWrapper);
    if(sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
    }
    if(window2) {
        SDL_DestroyWindow(window2);
    }
    if(sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
    }
    SDL_Quit();

    return 0;
}