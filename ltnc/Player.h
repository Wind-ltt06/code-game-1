#ifndef Player_h
#define Player_h

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"
#include <cmath>

class PlayerTank {
public:
    int x, y;           // Quay lại sử dụng int để đơn giản hóa vấn đề
    int dirX, dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    const int maxBullets = 5;
    const Uint32 reloadTime = 15000;
    Uint32 lastShotTime = 0;

    int speed;          // Tốc độ di chuyển cố định
    bool keyPressed;    // Kiểm tra xem có phím nào được nhấn không

    PlayerTank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 3;      // Tốc độ vừa phải
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;
        keyPressed = false;
    }

    void move(int dx, int dy, const std::vector<Wall>& walls) {
        // Nếu có thay đổi hướng, cập nhật hướng của tank
        if (dx != 0 || dy != 0) {
            dirX = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
            dirY = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
        }

        // Tính toán vị trí mới
        int newX = x + dx * speed;
        int newY = y + dy * speed;

        // Kiểm tra va chạm với tường theo từng trục
        bool canMoveX = true;
        bool canMoveY = true;

        SDL_Rect tempRectX = {newX, y, TILE_SIZE, TILE_SIZE};
        SDL_Rect tempRectY = {x, newY, TILE_SIZE, TILE_SIZE};

        for (const auto& wall : walls) {
            if (!wall.active) continue;

            if (SDL_HasIntersection(&tempRectX, &wall.rect)) {
                canMoveX = false;
            }

            if (SDL_HasIntersection(&tempRectY, &wall.rect)) {
                canMoveY = false;
            }
        }

        // Kiểm tra biên màn hình và cập nhật vị trí
        if (canMoveX && newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2) {
            x = newX;
        }

        if (canMoveY && newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
            y = newY;
        }

        // Cập nhật rect
        rect.x = x;
        rect.y = y;
    }

    void shoot() {
        Uint32 currentTime = SDL_GetTicks();

        if (bullets.size() >= maxBullets) {
            if (currentTime - lastShotTime < reloadTime) {
                return;
            }
            bullets.clear();
        }

        lastShotTime = currentTime;

        // Đảm bảo viên đạn bay theo hướng của tank
        bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY, 10));
    }

    void updateBullets(const std::vector<Wall>& walls) {
        for (auto& bullet : bullets) {
            bullet.move(walls);
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for (auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
};

#endif // Player_h
