
#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"

class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    //....

// create environtment
    Game() {
        running = true;
        if ( SDL_Init(SDL_INIT_VIDEO) < 0){
            cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() <<endl;
            running = false;
        }
        window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if(!window){
            cerr << "Window could not be created! SDL_Error: " << SDL_GetError() <<endl;
            running = false;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer){
            cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
            running = false;
        }
    }
    void render(){
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // boundaries
        SDL_RenderClear(renderer); // DELETE COLOR

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for ( int i = 1; i < MAP_HEIGHT - 1; ++i){
            for ( int j = 1; j < MAP_WIDTH - 1; ++j){
                SDL_Rect TILE = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderFillRect(renderer, &TILE);
            }
        }
        SDL_RenderPresent(renderer);
    }
    void run () {
        while(running) {
            render();
            SDL_Delay(16);
        }
    }
    ~Game(){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

    }

};


#endif
