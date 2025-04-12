#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include <iostream>
#include <bits/stdc++.h>
#include "Wall.h"
#include "Player.h"
#include "enemytank.h"
using namespace std;

class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
    PlayerTank player{(MAP_WIDTH - 1) / 2 * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE};
    int enemyNumber = 5;
    vector<EnemyTank> enemies;

// create environtment
  Game()
    : player(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE) {
    running = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }

    // Khởi tạo SDL_Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
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

    // Load texture cho player
    if (!player.loadTextures(renderer)) {
        cerr << "Failed to load player textures!" << endl;
        running = false;
    }

    generateWalls();
    spawnEnemies();
}

     void generateWalls(){
        for( int i = 3; i < MAP_HEIGHT - 3; i += 2){
            for ( int j = 3; j < MAP_WIDTH - 3; j += 2){
                Wall w = Wall(j * TILE_SIZE, i* TILE_SIZE);
                walls.push_back(w);
            }
        }
    }
  void handleEvents() {
    SDL_Event event;

    // Biến để theo dõi phím đang được nhấn
    int dx = 0, dy = 0;

    // Lấy trạng thái của tất cả các phím
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Kiểm tra các phím mũi tên và cập nhật dx, dy
    if (keystate[SDL_SCANCODE_UP]) {
        dy = -1;
        dx = 0;
    }
    else if (keystate[SDL_SCANCODE_DOWN]) {
        dy = 1;
        dx = 0;
    }
    else if (keystate[SDL_SCANCODE_LEFT]) {
        dx = -1;
        dy = 0;
    }
    else if (keystate[SDL_SCANCODE_RIGHT]) {
        dx = 1;
        dy = 0;
    }

    // Di chuyển tank theo dx, dy
    player.move(dx, dy, walls);

    // Xử lý các sự kiện khác
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    player.shoot();
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
            }
        }
    }
}


    void update(){

    player.updateBullets(walls);

        for(auto& bullet : player.bullets){
            for ( auto& wall : walls){
                if( wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)){
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
        for ( auto &bullet : player.bullets){
            for(auto& enemy : enemies){
                if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)){
                    enemy.active = false;
                    bullet.active = false;
                }
            }
        }

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](EnemyTank &e) {return !e.active;}), enemies.end());

        if(enemies.empty()){
            running = false;
        }

        for (auto &enemy : enemies) {
    enemy.move(walls, player.x, player.y, player.bullets);
    enemy.updateBullets(walls);
    if (rand() % 100 < 90) {  // Xác suất 1% để bắn
        enemy.shoot();
    }
}

        for(auto& enemy : enemies){
            for ( auto& bullet : enemy.bullets){
                for(auto& wall : walls){
                    if( wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)){
                        wall.active = false;
                        bullet.active = false;
                        break;
                    }
                }
            }
        }

        for( auto &enemy : enemies){
            for(auto& bullet : enemy.bullets){
                //update
                if(SDL_HasIntersection(&bullet.rect, &player.rect)){
                    running = false;
                    return;
                }
            }
        }

    }

    void spawnEnemies() {
    enemies.clear();
    for (int i = 0; i < enemyNumber; i++) {
        int ex, ey;
        bool validPosition = false;
        while(!validPosition) {
            ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;

            // Kiểm tra xem vị trí có phù hợp với kích thước tank mới không
            validPosition = true;
            SDL_Rect tempRect = {ex, ey, 30, 30};

            for (const auto& wall : walls) {
                if(wall.active && SDL_HasIntersection(&tempRect, &wall.rect)) {
                    validPosition = false;
                    break;
                }
            }

            // Kiểm tra khoảng cách với player
            int distToPlayer = abs(ex - player.x) + abs(ey - player.y);
            if (distToPlayer < 4 * TILE_SIZE) {
                validPosition = false;
            }
        }
     enemies.push_back(EnemyTank(ex, ey, renderer));

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

        for( auto &enemy : enemies){
            enemy.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

    void run () {
        while(running) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }
    }
    ~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();  // Giải phóng SDL_Image
    SDL_Quit();

    }

};


#endif
