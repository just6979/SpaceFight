#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <cmath>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include <Logging.h>
#include <inih/ini.h>
#include <inih/cpp/INIReader.h>

#include <GameSprite.h>
#include <Console.h>
#include <GamePlayer.h>

class Game {
public:
    static Game& getGame();
    bool init(const std::string& _name);
    void run();
private:
    bool initialized = false;
    Logger logger;
    struct {
        std::string name;
        // this size fits in most screens in windowed mode
        uint32_t width = 1200;
        uint32_t height = 675;
        // don't start fullscreen
        bool fullscreen = false;
        // but do use desktop size when going fullscreen
        bool useDesktopSize = true;
        // show the console
        bool showConsole = false;

        double deadZone = 15;
        double keySpeed = 75;
    } config;
    // where everything is drawn
    sf::RenderWindow window;
    // controls the 2D camera, used for rendering internally at a set size
    sf::View view;
    // where we render the game before copying it to the window
    sf::RenderTexture screen;
    // render internally to 720p widescreen
    uint32_t renderWidth = 1280;
    uint32_t renderHeight = 720;
    // displays messages over the playing screen
    Console& console = Console::getConsole();
    // all normal sprites to draw
    std::vector<GameSprite> sprites;
    // the player's ship sprite
    GamePlayer player;

    Game() { };
    // [re]create the rendering window, possibly fullscreen
    void createWindow(bool shouldFullscreen = false);
    // change the viewport to maintain 16:9 aspect ratio
    void adjustAspect(sf::Event::SizeEvent newSize);
    void adjustAspect(sf::Vector2u newSize);
    // main event loop
    void processEvents();
    // event handlers
    void handleKeyPress(const sf::Event& event);
    void handleKeyRelease(const sf::Event& event) const;
    // update the simulation
    void updateControls();
    void updateWorld(sf::Time elapsed);
    // render graphics based on simulation
    void renderWorld();
};
