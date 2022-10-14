//
// Created by hurin on 10/10/2022.
//

#include "stdio.h"

#include "SDL.h"


int main(int argc, char** argv) {

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
        goto QuitWindow1;
    }

    SDL_Renderer * sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if(NULL == sdlRenderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        goto QuitWindow2;
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
    SDL_Delay(3000);

    QuitRenderer:SDL_DestroyRenderer(sdlRenderer);
    QuitWindow2:SDL_DestroyWindow(window2);
    QuitWindow1:SDL_DestroyWindow(sdlWindow);
    Quit:SDL_Quit();

    return 0;
}