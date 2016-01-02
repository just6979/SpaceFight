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
    bool init(const std::string& _name);
    void run();
private:
    static Game* instance;
    static bool initialized;

    Game() { };
    struct {
        std::string name;
        unsigned int width = 1200;
        unsigned int height = 675;
        bool fullscreen = false;
        bool useDesktopSize = true;
        bool hideMouseFullscreen = true;
        double deadZone = 15;
        double keySpeed = 75;
    } config;
    sf::RenderWindow window;
    sf::View view;
    Console* console = NULL;
    GameSprite* player;
    std::vector<GameSprite*> sprites;
// system functions
    void adjustAspect(unsigned int width, unsigned int height);
    void resizeWindow(unsigned int width, unsigned int height, bool shouldFullscreen = false);
    void processEvents();
    void updateControls();
    void updateWorld(sf::Time elapsed);
    void renderWorld();
};

#endif // GAME_H
