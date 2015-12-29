#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <cmath>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include <easylogging++.h>
#include <ini.h>
#include <INIReader.h>

#include <GameSprite.h>

class Game {
public:
    static Game *getGame();
    bool init(const std::string &_name);
    void run();
private:
    static Game *instance;

    Game() { };
    std::string name;
    struct {
        unsigned int width = 1200;
        unsigned int height = 675;
        bool fullscreen = false;
        bool useDesktopSize = true;
    } config;
    float xScale, yScale = 0;
    sf::VideoMode desktop;
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
