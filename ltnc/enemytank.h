#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"
#include <algorithm>
#include <cstdlib>

class EnemyTank {
public:
    int x, y;
    int dirX, dirY;
    int moveDelay, shootDelay;
    SDL_Rect rect;
    bool active;
    std::vector<Bullet> bullets;


    // Hàm khởi tạo (constructor)
    EnemyTank(int startX, int startY) {
        moveDelay = 15; // Delay for movement
        shootDelay = 5;  // Delay for shooting
        x = startX;
        y = startY;
        rect = { x, y, TILE_SIZE, TILE_SIZE };
        dirX = 0;
        dirY = 1;
        active = true;
    }

    void move(const std::vector<Wall>& walls) {
        if (--moveDelay > 0)
            return;
        moveDelay = 15;

        int r = rand() % 4;
        if (r == 0) { // up
            dirX = 0;
            dirY = -5;
        }
        else if (r == 1) { // down
            dirX = 0;
            dirY = 5;
        }
        else if (r == 2) { // left
            dirY = 0;
            dirX = -5;
        }
        else if (r == 3) { // right
            dirY = 0;
            dirX = 5;
        }

        int newX = x + dirX;
        int newY = y + dirY;
        SDL_Rect newRect = { newX, newY, TILE_SIZE, TILE_SIZE };

        for (const auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                return;  // Va chạm với tường, không di chuyển
            }
        }
        if (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
            newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
        }
    }

    void shoot() {
        if (--shootDelay > 0)
            return;
        shootDelay = 5;
        // Sử dụng đúng tên vector là 'bullets'
        bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY));
    }

    void updateBullets() {
        std::vector <Wall> walls;
        for (auto &bullet : bullets) {
            bullet.move(walls);
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet &b) {
            return !b.active;
        }), bullets.end());
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for (auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
};

#endif // ENEMYTANK_H
