#ifndef Player_h
#define Player_h
#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"
#include <cmath>
#include <string>

class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    SDL_Texture* texture;
    SDL_Texture* upTexture;
    SDL_Texture* downTexture;
    SDL_Texture* leftTexture;
    SDL_Texture* rightTexture;
    SDL_Texture* bulletTexture;
    const int TANK_WIDTH = 30;
    const int TANK_HEIGHT = 30;

    std::vector<Bullet> bullets;
    const int maxBullets = 5;
    const Uint32 reloadTime = 3000;
    Uint32 lastShotTime = 0;
    int bulletsShot = 0;
    int speed;
    bool keyPressed;

    PlayerTank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 3;
        rect = {x, y, TANK_WIDTH, TANK_HEIGHT};
        dirX = 0;
        dirY = -1;
        keyPressed = false;
        texture = nullptr;
        upTexture = nullptr;
        downTexture = nullptr;
        leftTexture = nullptr;
        rightTexture = nullptr;
        bulletsShot = 0;
        lastShotTime = 0;
    }

    // Hàm để load các texture cho tank
    bool loadTextures(SDL_Renderer* renderer) {

        if (upTexture) SDL_DestroyTexture(upTexture);
        if (downTexture) SDL_DestroyTexture(downTexture);
        if (leftTexture) SDL_DestroyTexture(leftTexture);
        if (rightTexture) SDL_DestroyTexture(rightTexture);



        upTexture = loadTexture(renderer, "player/tankup.jpg");
        downTexture = loadTexture(renderer, "player/tankdown.jpg");
        leftTexture = loadTexture(renderer, "player/tankleft.jpg");
        rightTexture = loadTexture(renderer, "player/tankright.jpg");
         bulletTexture = loadTexture(renderer, "player/bullet.png");


        texture = upTexture;


        return (upTexture && downTexture && leftTexture && rightTexture);
    }

    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Không thể load ảnh %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
            return nullptr;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!tex) {
            SDL_Log("Không thể tạo texture từ %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        return tex;
    }

    void move(int dx, int dy, const std::vector<Wall>& walls) {

        if (dx != 0 || dy != 0) {
            dirX = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
            dirY = dy > 0 ? 1 : (dy < 0 ? -1 : 0);


            if (dirX == 0 && dirY == -1) texture = upTexture;
            else if (dirX == 0 && dirY == 1) texture = downTexture;
            else if (dirX == -1 && dirY == 0) texture = leftTexture;
            else if (dirX == 1 && dirY == 0) texture = rightTexture;
        }


        int newX = x + dx * speed;
        int newY = y + dy * speed;

        bool canMoveX = true;
        bool canMoveY = true;

    SDL_Rect tempRectX = {newX, y, TANK_WIDTH, TANK_HEIGHT};
    SDL_Rect tempRectY = {x, newY, TANK_WIDTH, TANK_HEIGHT};

        for (const auto& wall : walls) {
            if (!wall.active) continue;
            if (SDL_HasIntersection(&tempRectX, &wall.rect)) {
                canMoveX = false;
            }
            if (SDL_HasIntersection(&tempRectY, &wall.rect)) {
                canMoveY = false;
            }
        }


        if (canMoveX && newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2) {
            x = newX;
        }
        if (canMoveY && newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
            y = newY;
        }


          rect = {x, y, TANK_WIDTH, TANK_HEIGHT};
    }

 void shoot() {
    Uint32 currentTime = SDL_GetTicks();

    if (bulletsShot >= maxBullets) {

        if (currentTime - lastShotTime < reloadTime) {
            return;
        }
        bulletsShot = 0;
        lastShotTime = currentTime;
    }

    if (currentTime - lastShotTime < 200) {
        return;
    }

    bullets.push_back(Bullet(x + TANK_WIDTH/2 - 5, y + TANK_HEIGHT/2 - 5, dirX, dirY, 3, bulletTexture));
    lastShotTime = currentTime;
    bulletsShot++;
}


    void updateBullets(const std::vector<Wall>& walls) {
        for (auto& bullet : bullets) {
            bullet.move(walls);
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());
    }

    void render(SDL_Renderer* renderer) {
        if (texture) {

            SDL_RenderCopy(renderer, texture, NULL, &rect);
        } else {

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }


        for (auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }

    // Hàm hủy để giải phóng bộ nhớ
    ~PlayerTank() {
        if (upTexture) SDL_DestroyTexture(upTexture);
        if (downTexture) SDL_DestroyTexture(downTexture);
        if (leftTexture) SDL_DestroyTexture(leftTexture);
        if (rightTexture) SDL_DestroyTexture(rightTexture);

    }
};
#endif // Player_h
