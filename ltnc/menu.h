
#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "defs.h"
#include <SDL_ttf.h>
#include <bits/stdc++.h>

using namespace std;

class MenuItem {
public:
    std::string text;
    SDL_Rect rect;
    bool selected;

    MenuItem(const std::string& itemText, int x, int y, int width, int height) {
        text = itemText;
        rect = {x, y, width, height};
        selected = false;
    }
};

class Menu {
private:
    SDL_Texture* backgroundTexture;
    SDL_Texture* titleTexture;
    std::vector<MenuItem> menuItems;
    int selectedItem;
    TTF_Font* font;
    SDL_Renderer* renderer;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);

public:
    enum MenuResult {
        NOTHING,
        EXIT,
        PLAY,
        SETTINGS
    };

    Menu(SDL_Renderer* renderer);
    ~Menu();

    bool loadMedia();
    MenuResult handleEvents();
    void render();
};

#endif // MENU_H
