
#ifndef Bullet_H
#define Bullet_H

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>




class Bullet {
public:
    int x, y;
    int dx, dy;
    int speed;
    SDL_Rect rect;
    bool active;
    SDL_Texture* texture;

 Bullet(int startX, int startY, int dirX, int dirY, int bulletSpeed = 10, SDL_Texture* bulletTexture = nullptr) {
    x = startX;
    y = startY;
    dx = dirX;
    dy = dirY;
    speed = bulletSpeed;
    active = true;
    rect = {x, y, 10, 10};
    texture = bulletTexture;
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
            if (texture) {
                // Render với texture
                SDL_RenderCopy(renderer, texture, NULL, &rect);
            } else {
                // Sử dụng hình chữ nhật đơn giản nếu không có texture
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

};
   inline SDL_Texture* loadBulletTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_Log("Cannot load bullet image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!tex) {
        SDL_Log("Cannot create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }

    return tex;
}
#endif
