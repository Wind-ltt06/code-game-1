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
    SDL_Texture* texture;  // Thêm texture cho enemy tank

    SDL_Texture* upTexture;
    SDL_Texture* downTexture;
    SDL_Texture* leftTexture;
    SDL_Texture* rightTexture;
    SDL_Texture* bulletTexture;
    bool active;
    std::vector<Bullet> bullets;


   EnemyTank(int startX, int startY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    rect = { x, y, 30, 30 };
    dirX = 0;
    dirY = 1;
    active = true;
    moveCooldown = 15;
    shootDelay = 30;


    upTexture = loadTexture(renderer, "player/enermytankup.jpg");
    downTexture = loadTexture(renderer, "player/enermytankdown.jpg");
    leftTexture = loadTexture(renderer, "player/enermytankleft.jpg");
    rightTexture = loadTexture(renderer, "player/enermytankright.jpg");
    bulletTexture = loadTexture(renderer, "player/bullet.png");

    texture = downTexture;

    if (!upTexture || !downTexture || !leftTexture || !rightTexture) {
        SDL_Log("❌ NO");
    }
}
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_Log("❌ NO", path.c_str(), IMG_GetError());
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}



   bool isDangerNearby(const std::vector<Bullet>& playerBullets) {
    for (const auto& bullet : playerBullets) {
        if (!bullet.active) continue;

        int distX = bullet.x - x;
        int distY = bullet.y - y;


        int futureX = bullet.x + bullet.dx * 10;
        int futureY = bullet.y + bullet.dy * 10;

        bool willHitX = (bullet.dx > 0 && x > bullet.x && x < futureX) ||
                        (bullet.dx < 0 && x < bullet.x && x > futureX);
        bool willHitY = (bullet.dy > 0 && y > bullet.y && y < futureY) ||
                        (bullet.dy < 0 && y < bullet.y && y > futureY);

        if (bullet.dx != 0 && abs(y - bullet.y) < TILE_SIZE && willHitX) {
            return true;
        }


        if (bullet.dy != 0 && abs(x - bullet.x) < TILE_SIZE && willHitY) {
            return true;
        }
    }
    return false;
}

void move(const std::vector<Wall>& walls, int playerX, int playerY, const std::vector<Bullet>& playerBullets) {
    if (moveCooldown > 0) {
        moveCooldown--;
        return;
    }
    moveCooldown = 15;

     updateTextureBasedOnDirection();

    bool isDanger = isDangerNearby(playerBullets);


    std::vector<std::pair<int, int>> directions = {{0, -10}, {0, 10}, {-10, 0}, {10, 0}};

    std::random_shuffle(directions.begin(), directions.end());

    if (isDanger) {
        int bestDx = 0, bestDy = 0;
        int safestDistance = 0;

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
                int safetyScore = 0;
                for (const auto& bullet : playerBullets) {
                    if (!bullet.active) continue;
                    int bulletDistX = abs(bullet.x - newX);
                    int bulletDistY = abs(bullet.y - newY);
                    safetyScore += bulletDistX + bulletDistY;


                    if ((bullet.dx > 0 && dx > 0) ||
                        (bullet.dx < 0 && dx < 0) ||
                        (bullet.dy > 0 && dy > 0) ||
                        (bullet.dy < 0 && dy < 0)) {
                        safetyScore -= 50;
                    }


                    if ((bullet.dx != 0 && dy != 0) || (bullet.dy != 0 && dx != 0)) {
                        safetyScore += 50;
                    }
                }

                if (safetyScore > safestDistance) {
                    safestDistance = safetyScore;
                    bestDx = dx;
                    bestDy = dy;
                }
            }
        }


        if (bestDx != 0 || bestDy != 0) {
            x += bestDx;
            y += bestDy;
            rect.x = x;
            rect.y = y;
            dirX = (bestDx != 0) ? bestDx / abs(bestDx) : 0;
            dirY = (bestDy != 0) ? bestDy / abs(bestDy) : 0;
        }
    } else {

        if (rand() % 100 < 50) {
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


            if (bestDx != 0 || bestDy != 0) {
                x += bestDx;
                y += bestDy;
                rect.x = x;
                rect.y = y;
                dirX = (bestDx != 0) ? bestDx / abs(bestDx) : 0;
                dirY = (bestDy != 0) ? bestDy / abs(bestDy) : 0;
            }
        } else {
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
            void updateTextureBasedOnDirection() {
    SDL_Texture* newTexture = texture;

    if (dirX == 0 && dirY < 0) {
        newTexture = upTexture;
    } else if (dirX == 0 && dirY > 0) {
        newTexture = downTexture;
    } else if (dirX < 0 && dirY == 0) {
        newTexture = leftTexture;
    } else if (dirX > 0 && dirY == 0) {
        newTexture = rightTexture;
    }

    if (newTexture != texture) {
        texture = newTexture;

    }
}


void shoot() {
    if (shootDelay > 0) {
        shootDelay--;
        return;
    }
    shootDelay = 30;
    bullets.push_back(Bullet(x + 15, y + 15, dirX, dirY, 10, bulletTexture));
}

    void updateBullets(const std::vector<Wall>& walls) {
    for (auto &bullet : bullets) {
        bullet.move(walls);
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet &b) {
        return !b.active;
    }), bullets.end());
}



   void render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }


    for (auto &bullet : bullets) {
        bullet.render(renderer);
    }
}


};

#endif // ENEMYTANK_H
