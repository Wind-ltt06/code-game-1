#ifndef WALL_H
#define WALL_H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include <string>

class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;
    SDL_Texture* texture;

    Wall(int startX, int startY, SDL_Renderer* renderer) {
        x = startX;
        y = startY;
        active = true;
        rect = {x, y, TILE_SIZE, TILE_SIZE};


        texture = loadTexture(renderer, "player/wall.jpg");

        if (!texture) {
            SDL_Log("❌ Không thể load texture cho tường!");
        }
    }

    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("❌ Không thể load ảnh %s! Lỗi: %s\n", path.c_str(), IMG_GetError());
            return nullptr;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return tex;
    }

    void render(SDL_Renderer* renderer) {
        if (active) {
            if (texture) {
                SDL_RenderCopy(renderer, texture, NULL, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);  // fallback màu nâu
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

};

#endif  // WALL_H
