
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

class PlayerTank{
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;

    PlayerTank (int startX, int startY){
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;
    }
    void move(int dx, int dy, const vector<Wall>& walls) {
        int newX = x + dx;
        int newY = y + dy;
        this->dirX = dx;
        this->dirY = dy;

        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
        for (int i =0 ; i < walls.size(); i++){
            if( walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect)){
                return;
            }
        }
        if(newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
           newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2){
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
           }
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
PlayerTank player{(MAP_WIDTH - 1) / 2 * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE};
    //....

// create environtment
  Game()
    : player(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE) {
    running = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }

    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
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
    void handleEvents(){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN){
                running = false;
            }
            else if( event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym){
                    case SDLK_UP: player.move(0, -5, walls);break;
                    case SDLK_DOWN: player.move(0, 5, walls);break;
                    case SDLK_LEFT: player.move(-5, 0, walls);break;
                    case SDLK_RIGHT: player.move(5, 0, walls);break;
                    case SDLK_ESCAPE: running = false; break;
                }
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
        player.render(renderer);

        SDL_RenderPresent(renderer);
    }

    void run () {
        while(running) {
            handleEvents();
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
