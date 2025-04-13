
#ifndef Bullet_H
#define Bullet_H

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"



class Bullet {
public:
    int x, y;
    int dx, dy;
    int speed; // Thêm biến tốc độ
    SDL_Rect rect;
    bool active;

    Bullet(int startX, int startY, int dirX, int dirY, int bulletSpeed = 10) {
        x = startX;
        y = startY;
        dx = dirX;
        dy = dirY;
        speed = bulletSpeed; // Lưu tốc độ vào biến
        active = true;
        rect = {x, y, 10, 10}; // Kích thước viên đạn
    }

  void move(const std::vector<Wall>& walls) {
     x += dx * (speed * 0.5);
    y += dy * (speed * 0.5);
    rect.x = x;
    rect.y = y;

    // Kiểm tra va chạm với tường
    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&rect, &wall.rect)) {
            active = false; // Đạn biến mất, nhưng tường vẫn còn
            return;
        }
    }

    // Nếu ra ngoài màn hình thì biến mất
    if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE ||
        y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE) {
        active = false;
    }
}


    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};
#endif
