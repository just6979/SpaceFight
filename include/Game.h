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
        // this size fits in most screens in windowed mode
        unsigned int width = 1200;
        unsigned int height = 675;
        // render internally to 720p widescreen
        unsigned int renderWidth = 1280;
        unsigned int renderHeight = 720;
        // don't start fullscreen
        bool fullscreen = false;
        // but do use desktop size when going fullscreen
        bool useDesktopSize = true;
        bool hideMouseFullscreen = true;
        double deadZone = 15;
        double keySpeed = 75;
    } config;
    // where everything is drawn
    sf::RenderWindow window;
    // controls the 2D camera, used for rendering internally at a set size
    sf::View view;
    // displays messages over the playing screen
    Console* console = NULL;
    // all normal sprites to draw
    std::vector<GameSprite*> sprites;
    // the player's ship sprite
    GameSprite* player;

    // change the viewport to maintain 16:9 aspect ratio
    void adjustAspect(unsigned int width, unsigned int height);
    // [re]create the rendering window, possibly fullscreen
    void createWindow(bool shouldFullscreen = false);
    void processEvents();
    void updateControls();
    void updateWorld(sf::Time elapsed);
    void renderWorld();
};

#endif // GAME_H
