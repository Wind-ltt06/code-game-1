#include "Menu.h"
#include <SDL_ttf.h>
#include <iostream>

Menu::Menu(SDL_Renderer* rendererParam) {
    renderer = rendererParam;
    backgroundTexture = nullptr;
    titleTexture = nullptr;
    selectedItem = 0;
    font = nullptr;

    // Khởi tạo các mục menu
    menuItems.push_back(MenuItem("New Game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 40));
    menuItems.push_back(MenuItem("Setting", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 150, 40));
    menuItems.push_back(MenuItem("Exit", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 100, 40));

    menuItems[0].selected = true;
}

Menu::~Menu() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    if (titleTexture) {
        SDL_DestroyTexture(titleTexture);
    }
    if (font) {
        TTF_CloseFont(font);
    }
}

bool Menu::loadMedia() {
    // Khởi tạo SDL_ttf
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf không thể khởi tạo! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load font
    font = TTF_OpenFont("font/arial.ttf", 24);
    if (!font) {
        std::cerr << "Không thể load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load background
    SDL_Surface* surface = IMG_Load("player/menu1.jpg");
    if (!surface) {
        std::cerr << "Không thể load ảnh menu background! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Load hoặc tạo title texture
    SDL_Color titleColor = {255, 255, 0, 255}; // Màu vàng
    titleTexture = createTextTexture("BATTLE CITY", titleColor);

    return (backgroundTexture != nullptr && titleTexture != nullptr);
}

SDL_Texture* Menu::createTextTexture(const std::string& text, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Không thể render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return textTexture;
}

Menu::MenuResult Menu::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return EXIT;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    menuItems[selectedItem].selected = false;
                    selectedItem = (selectedItem - 1 + menuItems.size()) % menuItems.size();
                    menuItems[selectedItem].selected = true;
                    break;

                case SDLK_DOWN:
                    menuItems[selectedItem].selected = false;
                    selectedItem = (selectedItem + 1) % menuItems.size();
                    menuItems[selectedItem].selected = true;
                    break;

                case SDLK_RETURN:
                case SDLK_SPACE:
                    // Xử lý lựa chọn menu
                    if (selectedItem == 0) {
                        return PLAY;
                    }
                    else if (selectedItem == 1) {
                        return SETTINGS;
                    }
                    else if (selectedItem == 2) {
                        return EXIT;
                    }
                    break;

                case SDLK_ESCAPE:
                    return EXIT;
                    break;
            }
        }
    }

    return NOTHING;
}

void Menu::render() {
    // Vẽ background
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    } else {
        // Nếu không có background, vẽ màu đen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    // Vẽ title
    if (titleTexture) {
        SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 4, 300, 60};
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    }

    // Vẽ các mục menu
    for (size_t i = 0; i < menuItems.size(); i++) {
        SDL_Color textColor = menuItems[i].selected ? SDL_Color{255, 255, 255, 255} : SDL_Color{150, 150, 150, 255};
        SDL_Texture* itemTexture = createTextTexture(menuItems[i].text, textColor);

        if (itemTexture) {
            SDL_RenderCopy(renderer, itemTexture, NULL, &menuItems[i].rect);
            SDL_DestroyTexture(itemTexture); // Giải phóng texture sau khi vẽ
        }

        // Vẽ hình chữ nhật xung quanh mục được chọn
        if (menuItems[i].selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
            SDL_Rect outlineRect = menuItems[i].rect;
            outlineRect.x -= 5;
            outlineRect.y -= 5;
            outlineRect.w += 10;
            outlineRect.h += 10;
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
    }

    // Hiển thị các thay đổi
    SDL_RenderPresent(renderer);
}
