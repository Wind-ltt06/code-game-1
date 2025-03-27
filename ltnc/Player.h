#ifndef Player_h
#define Player_h

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"

class PlayerTank{
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;

    PlayerTank (int startX, int startY){
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;
    }
    void move(int dx, int dy, const std::vector<Wall>& walls) {
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
    void shoot(){
        int bulletSpeed = 5S;
        bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE /2 - 5, this->dirX, this->dirY));
    }

    void updateBullets(){
        for (auto &bullet : bullets){
            bullet.move();
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet &b){return !b.active;}), bullets.end());
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for (auto &bullet : bullets){
            bullet.render(renderer);
        }
    }

};

#endif // Player_h
