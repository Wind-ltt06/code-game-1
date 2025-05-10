#include "Menu.h"
#include <SDL_ttf.h>
#include <iostream>

Menu::Menu(SDL_Renderer* rendererParam) {
    renderer = rendererParam;
    backgroundTexture = nullptr;
    titleTexture = nullptr;
    selectedItem = 0;
    selectedSettingsItem = 0;
    inSettings = false;
    font = nullptr;

    // Khởi tạo các mục menu chính
    menuItems.push_back(MenuItem("New Game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 40));
    menuItems.push_back(MenuItem("Setting", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 150, 40));
    menuItems.push_back(MenuItem("Exit", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 100, 40));

    // Khởi tạo các mục menu settings
    settingsItems.push_back(MenuItem("Sound Volume", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 250, 40));
    settingsItems.push_back(MenuItem("Back", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 250, 40));

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
            if (!inSettings) {

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
                        if (selectedItem == 0) return PLAY;
                        else if (selectedItem == 1) {
                            inSettings = true;
                            selectedSettingsItem = 0;
                            settingsItems[0].selected = true;
                            return SETTINGS;
                        }
                        else if (selectedItem == 2) return EXIT;
                        break;

                    case SDLK_ESCAPE:
                        return EXIT;
                        break;
                }
            } else {

                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        settingsItems[selectedSettingsItem].selected = false;
                        selectedSettingsItem = (selectedSettingsItem - 1 + settingsItems.size()) % settingsItems.size();
                        settingsItems[selectedSettingsItem].selected = true;
                        break;

                    case SDLK_DOWN:
                        settingsItems[selectedSettingsItem].selected = false;
                        selectedSettingsItem = (selectedSettingsItem + 1) % settingsItems.size();
                        settingsItems[selectedSettingsItem].selected = true;
                        break;

                    case SDLK_LEFT:
                        if (selectedSettingsItem == 0) {
                            settingsItems[0].value = max(0, settingsItems[0].value - 10);

                            int volume = (settingsItems[0].value * MIX_MAX_VOLUME) / 100;
                            Mix_VolumeMusic(volume);
                            if (settingsItems[0].value == 0) {
                                Mix_PauseMusic();
                            } else {
                                Mix_ResumeMusic();
                            }
                        }
                        break;

                    case SDLK_RIGHT:
                        if (selectedSettingsItem == 0) {
                            settingsItems[0].value = min(100, settingsItems[0].value + 10);
                            int volume = (settingsItems[0].value * MIX_MAX_VOLUME) / 100;
                            Mix_VolumeMusic(volume);
                            if (settingsItems[0].value > 0) {
                                Mix_ResumeMusic();
                            }
                        }
                        break;

                    case SDLK_RETURN:
                    case SDLK_SPACE:
                        if (selectedSettingsItem == 1) {
                            inSettings = false;
                            return BACK;
                        }
                        break;

                    case SDLK_ESCAPE:
                        inSettings = false;
                        return BACK;
                        break;
                }
            }
        }
    }

    return NOTHING;
}

void Menu::render() {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    if (!inSettings) {
        if (titleTexture) {
            SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 4, 300, 60};
            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        }
        for (size_t i = 0; i < menuItems.size(); i++) {
            SDL_Color textColor = menuItems[i].selected ? SDL_Color{255, 255, 255, 255} : SDL_Color{150, 150, 150, 255};
            SDL_Texture* itemTexture = createTextTexture(menuItems[i].text, textColor);

            if (itemTexture) {
                SDL_RenderCopy(renderer, itemTexture, NULL, &menuItems[i].rect);
                SDL_DestroyTexture(itemTexture);
            }

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
    } else {
        renderSettings();
    }

    SDL_RenderPresent(renderer);
}

void Menu::renderSettings() {
    SDL_Color titleColor = {255, 255, 0, 255};
    SDL_Texture* settingsTitle = createTextTexture("SETTINGS", titleColor);
    if (settingsTitle) {
        SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 4, 200, 40};
        SDL_RenderCopy(renderer, settingsTitle, NULL, &titleRect);
        SDL_DestroyTexture(settingsTitle);
    }


    for (size_t i = 0; i < settingsItems.size(); i++) {
        SDL_Color textColor = settingsItems[i].selected ? SDL_Color{255, 255, 255, 255} : SDL_Color{150, 150, 150, 255};


        std::string displayText = settingsItems[i].text;
        if (i == 0) {
            displayText += ": " + std::to_string(settingsItems[i].value) + "%";
        }

        SDL_Texture* itemTexture = createTextTexture(displayText, textColor);
        if (itemTexture) {
            SDL_RenderCopy(renderer, itemTexture, NULL, &settingsItems[i].rect);
            SDL_DestroyTexture(itemTexture);
        }


        if (i == 0) {

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_Rect barRect = {
                settingsItems[i].rect.x,
                settingsItems[i].rect.y + settingsItems[i].rect.h + 5,
                settingsItems[i].rect.w,
                10
            };
            SDL_RenderFillRect(renderer, &barRect);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            barRect.w = (settingsItems[i].value * settingsItems[i].rect.w) / 100;
            SDL_RenderFillRect(renderer, &barRect);
        }

        if (settingsItems[i].selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
            SDL_Rect outlineRect = settingsItems[i].rect;
            outlineRect.x -= 5;
            outlineRect.y -= 5;
            outlineRect.w += 10;
            outlineRect.h += 10;
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
    }
}
