
#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class Wall{
public:
    int x, y;
    SDL_Rect rect;
    bool active;

    Wall(int startX, int startY) {
        x = startX;
        y = startY;
        active = true;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
    }

    void render(SDL_Renderer* renderer){
        if(active){
            SDL_SetRenderDrawColor(renderer, 150, 75, 0 ,255);  //BROWN COLOR
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};


class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
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
        generateWalls();
    }
     void generateWalls(){
        for( int i = 3; i < MAP_HEIGHT - 3; i += 2){
            for ( int j = 3; j < MAP_WIDTH - 3; j += 2){
                Wall w = Wall(j * TILE_SIZE, i* TILE_SIZE);
                walls.push_back(w);
            }
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
        for ( int i = 0; i < walls.size(); i++){
            walls[i].render(renderer);
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
