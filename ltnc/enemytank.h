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
    int moveCooldown;
    int moveDelay, shootDelay;
    SDL_Rect rect;
    bool active;
    std::vector<Bullet> bullets;


    // Hàm khởi tạo (constructor)
    EnemyTank(int startX, int startY) {
        moveDelay = 15; // Delay for movement
        shootDelay = 1;  // Delay for shooting
        x = startX;
        y = startY;
        rect = { x, y, TILE_SIZE, TILE_SIZE };
        dirX = 0;
        dirY = 1;
        active = true;
        moveCooldown = 20;
    }

    void move(const std::vector<Wall>& walls, int playerX, int playerY) {
          if (moveCooldown > 0) {
            moveCooldown--;
            return;  // Chưa hết cooldown, không di chuyển
        }
        moveCooldown = 15;  // Reset cooldown (chỉnh số này để thay đổi tốc độ)

    int bestDx = 0, bestDy = 0;
    int shortestDistance = abs(x - playerX) + abs(y - playerY);

    // Thử 4 hướng: lên, xuống, trái, phải
    std::vector<std::pair<int, int>> directions = {{0, -5}, {0, 5}, {-5, 0}, {5, 0}};
    std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

    for (auto [dx, dy] : directions) {
        int newX = x + dx;
        int newY = y + dy;
        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};

        bool collision = false;
        for (const auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                collision = true;
                break;
            }
        }

        int newDistance = abs(newX - playerX) + abs(newY - playerY);
        if (!collision && newDistance < shortestDistance) {
            bestDx = dx;
            bestDy = dy;
            shortestDistance = newDistance;
        }
    }

    x += bestDx;
    y += bestDy;
    rect.x = x;
    rect.y = y;
}

    void shoot() {
        if (--shootDelay > 0)
            return;
        shootDelay = 0;
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
