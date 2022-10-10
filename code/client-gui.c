//
// Created by hurin on 10/10/2022.
//

#include "stdio.h"

#include <SDL.h>


int main(int argc, char** argv) {

    int init = SDL_Init(SDL_INIT_EVERYTHING);

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
        goto QuitWindow1;
    }

    SDL_Renderer * sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == window2)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        goto QuitWindow2;
    }
    SDL_Delay(3000);

    QuitRenderer:SDL_DestroyRenderer(sdlRenderer);
    QuitWindow2:SDL_DestroyWindow(window2);
    QuitWindow1:SDL_DestroyWindow(sdlWindow);
    Quit:SDL_Quit();

    return 0;
}