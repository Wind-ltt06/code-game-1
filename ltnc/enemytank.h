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
        shootDelay = 2;  // Delay for shooting
        x = startX;
        y = startY;
        rect = { x, y, TILE_SIZE, TILE_SIZE };
        dirX = 0;
        dirY = 1;
        active = true;
        moveCooldown = 15;
    }

    // Kiểm tra xem có đạn nào gần không
   bool isDangerNearby(const std::vector<Bullet>& playerBullets) {
    for (const auto& bullet : playerBullets) {
        if (!bullet.active) continue;

        // Tính khoảng cách từ đạn đến tank
        int distX = bullet.x - x;
        int distY = bullet.y - y;

        // Dự đoán vị trí viên đạn trong tương lai
        int futureX = bullet.x + bullet.dx * 10;  // Dự đoán 10 bước
        int futureY = bullet.y + bullet.dy * 10;

        // Kiểm tra nếu đạn sẽ gặp tank trong tương lai gần
        bool willHitX = (bullet.dx > 0 && x > bullet.x && x < futureX) ||
                        (bullet.dx < 0 && x < bullet.x && x > futureX);
        bool willHitY = (bullet.dy > 0 && y > bullet.y && y < futureY) ||
                        (bullet.dy < 0 && y < bullet.y && y > futureY);

        // Đạn đang đi theo chiều ngang và sẽ đi qua vị trí Y của tank
        if (bullet.dx != 0 && abs(y - bullet.y) < TILE_SIZE && willHitX) {
            return true;
        }

        // Đạn đang đi theo chiều dọc và sẽ đi qua vị trí X của tank
        if (bullet.dy != 0 && abs(x - bullet.x) < TILE_SIZE && willHitY) {
            return true;
        }
    }
    return false;
}

void move(const std::vector<Wall>& walls, int playerX, int playerY, const std::vector<Bullet>& playerBullets) {
    if (moveCooldown > 0) {
        moveCooldown--;
        return;  // Chưa hết cooldown, không di chuyển
    }
    moveCooldown = 15;  // Reset cooldown

    // Kiểm tra xem có đạn gần không
    bool isDanger = isDangerNearby(playerBullets);

    // Các hướng di chuyển: lên, xuống, trái, phải
    std::vector<std::pair<int, int>> directions = {{0, -5}, {0, 5}, {-5, 0}, {5, 0}};

    // Xáo trộn hướng di chuyển để tăng tính ngẫu nhiên
    std::random_shuffle(directions.begin(), directions.end());

    if (isDanger) {
        // Nếu có nguy hiểm, tìm hướng tốt nhất để né đạn
        int bestDx = 0, bestDy = 0;
        int safestDistance = 0;

        for (auto [dx, dy] : directions) {
            int newX = x + dx;
            int newY = y + dy;
            SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};

            // Kiểm tra va chạm với tường
            bool collision = false;
            for (const auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                    collision = true;
                    break;
                }
            }

            // Kiểm tra biên màn hình
            bool inBounds = (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
                            newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2);

            if (!collision && inBounds) {
                // Tính toán mức độ an toàn của vị trí mới
                int safetyScore = 0;

                for (const auto& bullet : playerBullets) {
                    if (!bullet.active) continue;

                    // Khoảng cách từ vị trí mới đến đạn
                    int bulletDistX = abs(bullet.x - newX);
                    int bulletDistY = abs(bullet.y - newY);

                    // Đạn càng xa càng an toàn
                    safetyScore += bulletDistX + bulletDistY;

                    // Tránh di chuyển theo hướng đạn đang bay tới
                    if ((bullet.dx > 0 && dx > 0) ||
                        (bullet.dx < 0 && dx < 0) ||
                        (bullet.dy > 0 && dy > 0) ||
                        (bullet.dy < 0 && dy < 0)) {
                        safetyScore -= 50; // Phạt nặng nếu di chuyển theo hướng đạn
                    }

                    // Thưởng nếu di chuyển vuông góc với hướng đạn
                    if ((bullet.dx != 0 && dy != 0) || (bullet.dy != 0 && dx != 0)) {
                        safetyScore += 30;
                    }
                }

                if (safetyScore > safestDistance) {
                    safestDistance = safetyScore;
                    bestDx = dx;
                    bestDy = dy;
                }
            }
        }

        // Di chuyển theo hướng an toàn nhất
        if (bestDx != 0 || bestDy != 0) {
            x += bestDx;
            y += bestDy;
            rect.x = x;
            rect.y = y;
            dirX = (bestDx != 0) ? bestDx / abs(bestDx) : 0;
            dirY = (bestDy != 0) ? bestDy / abs(bestDy) : 0;
        }
    } else {
        // Không có nguy hiểm, đi về phía người chơi với xác suất cao
        if (rand() % 100 < 70) { // 70% thời gian di chuyển về phía người chơi
            int bestDx = 0, bestDy = 0;
            int shortestDistance = abs(x - playerX) + abs(y - playerY);

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

                bool inBounds = (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
                                newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2);

                int newDistance = abs(newX - playerX) + abs(newY - playerY);
                if (!collision && inBounds && newDistance < shortestDistance) {
                    bestDx = dx;
                    bestDy = dy;
                    shortestDistance = newDistance;
                }
            }

            // Di chuyển theo hướng tốt nhất
            if (bestDx != 0 || bestDy != 0) {
                x += bestDx;
                y += bestDy;
                rect.x = x;
                rect.y = y;
                dirX = (bestDx != 0) ? bestDx / abs(bestDx) : 0;
                dirY = (bestDy != 0) ? bestDy / abs(bestDy) : 0;
            }
        } else {
            // 30% thời gian di chuyển ngẫu nhiên
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

                bool inBounds = (newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 &&
                                newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2);

                if (!collision && inBounds) {
                    x = newX;
                    y = newY;
                    rect.x = x;
                    rect.y = y;
                    dirX = (dx != 0) ? dx / abs(dx) : 0;
                    dirY = (dy != 0) ? dy / abs(dy) : 0;
                    break;
                }
            }
        }
    }
}

    void shoot() {
        if (shootDelay > 0) {
            shootDelay--;
            return;
        }
        shootDelay = 30; // Thời gian chờ giữa các lần bắn
        bullets.push_back(Bullet(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY));
    }

    void updateBullets() {
        std::vector<Wall> walls;
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
