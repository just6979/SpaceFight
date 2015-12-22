#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <cmath>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include "GameSprite.h"

class Game {
public:
    static Game *getGame();

    void init(const std::string &_name, unsigned int _width, unsigned int _height);

    void run();

private:
    static Game *instance;

    Game() { };
    std::string name;
    unsigned int width, height = 0;
    float xScale, yScale = 0;
    sf::VideoMode desktop;
    bool fullscreen = false;
    sf::RenderWindow *window;
    sf::RenderTexture *screen;
    sf::Sprite *screenSprite;
// game data
    const float deadZone = 15;
    const float keySpeed = 75;
    GameSprite *player;

// system functions
    void resizeWindow(bool fullscreen);
    void adjustScale();
    void processEvents();
    void updateControls();
    void updateWorld(sf::Time elapsed);
    void renderWorld();
};

#endif // GAME_H
