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
#include <Console.h>

class Game {
public:
    static Game* getGame();
    bool init(const std::string &_name);
    void run();
private:
    static Game* instance;
    static bool initialized;
    Game() { };
    struct {
        std::string name;
        unsigned short width = 1200;
        unsigned short height = 675;
        bool fullscreen = false;
        bool useDesktopSize = true;
        bool hideMouseFullscreen = true;
        double deadZone = 15;
        double keySpeed = 75;
    } config;
    sf::RenderWindow* window;
    Console* console = NULL;
    GameSprite* player;
    std::vector<GameSprite*> sprites;
// system functions
    void resizeWindow(bool shouldFullscreen);
    void processEvents();
    void updateControls();
    void updateWorld(sf::Time elapsed);
    void renderWorld();
};

#endif // GAME_H
