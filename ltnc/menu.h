#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "defs.h"
#include <SDL_ttf.h>
#include <bits/stdc++.h>
#include <SDL_mixer.h>

using namespace std;

class MenuItem {
public:
    std::string text;
    SDL_Rect rect;
    bool selected;
    int value;

    MenuItem(const std::string& itemText, int x, int y, int width, int height) {
        text = itemText;
        rect = {x, y, width, height};
        selected = false;
        value = 50;
    }
};

class Menu {
private:
    SDL_Texture* backgroundTexture;
    SDL_Texture* titleTexture;
    std::vector<MenuItem> menuItems;
    std::vector<MenuItem> settingsItems;
    int selectedItem;
    int selectedSettingsItem;
    TTF_Font* font;
    SDL_Renderer* renderer;
    bool inSettings;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void renderSettings();

public:
    enum MenuResult {
        NOTHING,
        EXIT,
        PLAY,
        SETTINGS,
        BACK
    };

    Menu(SDL_Renderer* renderer);
    ~Menu();

    bool loadMedia();
    MenuResult handleEvents();
    void render();
};

#endif // MENU_H
