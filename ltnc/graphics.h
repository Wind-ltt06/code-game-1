#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Wall.h"
#include "Player.h"
#include "enemytank.h"
#include "Menu.h"
#include <SDL_mixer.h>


using namespace std;

class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
    PlayerTank player{(MAP_WIDTH - 1) / 2 * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE};
    int enemyNumber = 7;
    vector<EnemyTank> enemies;
    SDL_Texture* mapTexture;
    TTF_Font* font;
    SDL_Texture* scoreTexture;
    SDL_Rect scoreRect;
    Mix_Music* backgroundMusic = nullptr;

    int score = 0;

    enum GameState {
        MENU,
        PLAYING,
        GAME_OVER
    };

          SDL_Texture* loadTexture(const std::string& path) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Không thể load ảnh %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
            return nullptr;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return tex;
    }
      GameState state;
    Menu* menu;

// create environtment
  Game()
    : player(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE) {
    running = true;
     mapTexture = nullptr;
     state = MENU;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    if (TTF_Init() < 0) {
    cerr << "SDL_ttf không thể khởi tạo! SDL_ttf Error: " << TTF_GetError() << endl;
    running = false;
}

    font = TTF_OpenFont("font/arial.ttf", 24);
        if (!font) {
    cerr << "Không thể load font score! SDL_ttf Error: " << TTF_GetError() << endl;
    running = false;
        }

    scoreTexture = nullptr;
    // hàm nhạc

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    cerr << "SDL_mixer không thể khởi tạo! Lỗi: " << Mix_GetError() << endl;
    running = false;
    }

    // Load nhạc nền
    backgroundMusic = Mix_LoadMUS("music/music.mp3");
    if (!backgroundMusic) {
    cerr << "❌ Không thể load nhạc nền! Lỗi: " << Mix_GetError() << endl;
    } else {
    Mix_PlayMusic(backgroundMusic, -1);
}

    // Khởi tạo SDL_Image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        running = false;
    }

    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }

     menu = new Menu(renderer);
        if (!menu->loadMedia()) {
            cerr << "Không thể load tài nguyên menu!" << endl;
            running = false;
        }

    // Load texture cho player
    if (!player.loadTextures(renderer)) {
        cerr << "Failed to load player textures!" << endl;
        running = false;
    }
          // Tải texture cho map
        mapTexture = loadTexture("player/map.jpg");
        if (!mapTexture) {
            SDL_Log("❌ Không thể load ảnh nền map!");
        }

    generateWalls();
    spawnEnemies();
        updateScoreTexture();
}
//hàm tạo map
void generateWalls(){
    for (int i = 3; i < MAP_HEIGHT - 3; i += 2){
        for (int j = 3; j < MAP_WIDTH - 3; j += 2){
            Wall w(j * TILE_SIZE, i * TILE_SIZE, renderer);
            walls.push_back(w);
        }
    }
}
// hàm điều khiển
    void handleEvents() {
        if (state == MENU) {
            Menu::MenuResult result = menu->handleEvents();
            switch (result) {
                case Menu::PLAY:
                    // bắt đầu lại game khi new game
                    state = PLAYING;
                    score = 0;
                    updateScoreTexture();
                    // Reset player position
                    player.x = ((MAP_WIDTH - 1) / 2) * TILE_SIZE;
                    player.y = (MAP_HEIGHT - 2) * TILE_SIZE;
                    player.bullets.clear();

                    walls.clear();
                    generateWalls();
                    enemies.clear();
                    spawnEnemies();
                    break;
                case Menu::EXIT:
                    running = false;
                    break;
                case Menu::SETTINGS:
                case Menu::BACK:
                    break;
                default:
                    break;
            }
        } else if (state == PLAYING) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            state = MENU;
                            break;
                    }
                }
            }

            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            int dx = 0, dy = 0;
            if (keystate[SDL_SCANCODE_UP]) dy = -1;
            else if (keystate[SDL_SCANCODE_DOWN]) dy = 1;
            else if (keystate[SDL_SCANCODE_LEFT]) dx = -1;
            else if (keystate[SDL_SCANCODE_RIGHT]) dx = 1;

            player.move(dx, dy, walls);

            if (keystate[SDL_SCANCODE_SPACE]) {
                player.shoot();
            }
        } else if (state == GAME_OVER) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_r:

                            state = PLAYING;
                            score = 0;
                            updateScoreTexture();

                            player.x = ((MAP_WIDTH - 1) / 2) * TILE_SIZE;
                            player.y = (MAP_HEIGHT - 2) * TILE_SIZE;
                            player.bullets.clear();

                            walls.clear();
                            generateWalls();
                            enemies.clear();
                            spawnEnemies();
                            break;
                        case SDLK_ESCAPE:
                            state = MENU;
                            break;
                    }
                }
            }
        }
    }

    void update(){
           if (state != PLAYING) return;

        player.updateBullets(walls);
        // va chạm giữa đạn và tường
        for(auto& bullet : player.bullets){
            for ( auto& wall : walls){
                if( wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)){
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
        // va chạm giữa đạn và enermy
        for ( auto &bullet : player.bullets){
            for(auto& enemy : enemies){
                if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)){
                    enemy.active = false;
                    bullet.active = false;
                    score += 100;
                    updateScoreTexture();

                    // Sinh thêm enemy tank mới
                    int ex, ey;
                    bool validPosition = false;
                    while(!validPosition) {
                        ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
                        ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;

                        // Kiểm tra xem vị trí có phù hợp với kích thước tank mới không
                        validPosition = true;
                        SDL_Rect tempRect = {ex, ey, 30, 30};

                        for (const auto& wall : walls) {
                            if(wall.active && SDL_HasIntersection(&tempRect, &wall.rect)) {
                                validPosition = false;
                                break;
                            }
                        }

                        // Kiểm tra khoảng cách với player
                        int distToPlayer = abs(ex - player.x) + abs(ey - player.y);
                        if (distToPlayer < 4 * TILE_SIZE) {
                            validPosition = false;
                        }

                        // Kiểm tra khoảng cách với các enemy khác
                        for (const auto& otherEnemy : enemies) {
                            if (otherEnemy.active) {
                                int distToEnemy = abs(ex - otherEnemy.x) + abs(ey - otherEnemy.y);
                                if (distToEnemy < 2 * TILE_SIZE) {
                                    validPosition = false;
                                    break;
                                }
                            }
                        }
                    }
                    enemies.push_back(EnemyTank(ex, ey, renderer));
                }
            }
        }

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](EnemyTank &e) {return !e.active;}), enemies.end());

        // va chạm giữa đạn enermy và người chơi
        for(auto& enemy : enemies){
            for(auto& bullet : enemy.bullets){
                if(SDL_HasIntersection(&bullet.rect, &player.rect)){
                    state = GAME_OVER;
                    return;
                }
            }
        }

        // tỉ lệ bắn dạn của enermy tank
        for (auto &enemy : enemies) {
            enemy.move(walls, player.x, player.y, player.bullets);
            enemy.updateBullets(walls);
            if (rand() % 100 < 90) {  // Xác suất 90% để bắn
                enemy.shoot();
            }
        }

        for(auto& enemy : enemies){
            for ( auto& bullet : enemy.bullets){
                for(auto& wall : walls){
                    if( wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)){
                        wall.active = false;
                        bullet.active = false;
                        break;
                    }
                }
            }
        }

        if(enemies.empty()){
            state = GAME_OVER;
        }
    }

    void updateScoreTexture() {
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);

    SDL_Color textColor = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);

    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!surface) {
        SDL_Log("Không thể tạo surface score! %s", TTF_GetError());
        return;
    }

    scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!scoreTexture) {
        SDL_Log("Không thể tạo texture score! %s", SDL_GetError());
        return;
    }

    scoreRect = {10, 10, surface->w, surface->h}; // Góc trái trên
}

// hàm sinh kẻ thù
    void spawnEnemies() {
    enemies.clear();
    for (int i = 0; i < enemyNumber; i++) {
        int ex, ey;
        bool validPosition = false;
        while(!validPosition) {
            ex = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            ey = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;

            // Kiểm tra xem vị trí có phù hợp với kích thước tank mới không
            validPosition = true;
            SDL_Rect tempRect = {ex, ey, 30, 30};

            for (const auto& wall : walls) {
                if(wall.active && SDL_HasIntersection(&tempRect, &wall.rect)) {
                    validPosition = false;
                    break;
                }
            }

            // Kiểm tra khoảng cách với player
            int distToPlayer = abs(ex - player.x) + abs(ey - player.y);
            if (distToPlayer < 4 * TILE_SIZE) {
                validPosition = false;
            }
        }
     enemies.push_back(EnemyTank(ex, ey, renderer));

    }
}



 void render() {
        if (state == MENU) {
            menu->render();
        } else if (state == PLAYING) {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            SDL_RenderClear(renderer);

            if (scoreTexture) {
                SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
            }

            // Vẽ map
            if (mapTexture) {
                SDL_Rect mapRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderCopy(renderer, mapTexture, NULL, &mapRect);
            }

            // Vẽ tường, người chơi và kẻ địch
            for (auto& wall : walls) {
                wall.render(renderer);
            }

            player.render(renderer);

            for (auto& enemy : enemies) {
                enemy.render(renderer);
            }

            SDL_RenderPresent(renderer);
        } else if (state == GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Vẽ text Game Over
            SDL_Color textColor = {255, 0, 0, 255}; // Màu đỏ
            std::string gameOverText = "GAME OVER";
            SDL_Surface* surface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {
                    SCREEN_WIDTH/2 - surface->w/2,
                    SCREEN_HEIGHT/3 - surface->h/2,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            // Vẽ các lựa chọn
            textColor = {255, 255, 255, 255}; // Màu trắng
            std::string restartText = "Press R to Restart";
            surface = TTF_RenderText_Solid(font, restartText.c_str(), textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {
                    SCREEN_WIDTH/2 - surface->w/2,
                    SCREEN_HEIGHT/2 - surface->h/2,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            std::string exitText = "Press ESC to Menu";
            surface = TTF_RenderText_Solid(font, exitText.c_str(), textColor);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {
                    SCREEN_WIDTH/2 - surface->w/2,
                    SCREEN_HEIGHT/2 + 50,
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            SDL_RenderPresent(renderer);
        }
    }

    void run () {
        while(running) {
            handleEvents();
            update();
            render();
            SDL_Delay(16);
        }
    }
    ~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();

    }

};


#endif
