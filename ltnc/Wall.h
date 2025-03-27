#ifndef WALL_H
#define WALL_H

#include <SDL.h>
#include "defs.h"

class Wall {
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

    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);  // Màu nâu
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

#endif  // WALL_H
