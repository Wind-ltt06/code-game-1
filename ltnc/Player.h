#ifndef Player_h
#define Player_h

#include <SDL.h>
#include "defs.h"
#include "Wall.h"
#include <vector>
#include "Bullet.h"

class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    const int maxBullets = 5;  // Giới hạn số viên đạn
    const Uint32 reloadTime = 15000; // Thời gian hồi (milliseconds)
    Uint32 lastShotTime = 0;

    PlayerTank (int startX, int startY) {
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
        for (const auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                return;
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
        Uint32 currentTime = SDL_GetTicks();

    // Nếu đã bắn đủ 5 viên và chưa hồi xong thì không bắn
    if (bullets.size() >= maxBullets) {
        if (currentTime - lastShotTime < reloadTime) {
            return; // Chưa hết thời gian hồi, không bắn
        }
        // Nếu đã hết thời gian hồi, reset băng đạn
        bullets.clear();
    }

    // Cập nhật thời gian bắn cho mỗi viên
    lastShotTime = currentTime;

    // Tạo viên đạn mới
    double bulletSpeed = 1.0;
    bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX * bulletSpeed, dirY * bulletSpeed));
}


    void updateBullets(const std::vector<Wall>& walls) {
    for (auto& bullet : bullets) {
        bullet.move(walls);

        // Kiểm tra va chạm với tường
        for (const auto& wall : walls) {
            if (SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                bullet.active = false; // Chỉ vô hiệu hóa đạn, không thay đổi tường
                break; // Thoát vòng lặp sau khi va vào tường
            }
        }
    }

    // Xóa các viên đạn đã bị vô hiệu hóa
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
