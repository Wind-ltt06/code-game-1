#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "graphics.h"
#include "defs.h"
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char *argv[])
{
    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
