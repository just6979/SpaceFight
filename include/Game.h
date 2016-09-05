#pragma once

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include <inih/ini.h>
#include <inih/cpp/INIReader.h>
#include <logog/include/logog.hpp>

#include <GamePlayer.h>
#include <GameSprite.h>

class Game {
public:
    static Game& getGame(const std::string& _name = NULL);
    bool ready();
    void run();
private:
    bool isReady = false;
    struct {
        std::string name;
        // this size fits in most screens in windowed mode
        unsigned int width = 1200;
        unsigned int height = 675;
        // don't start fullscreen
        bool fullscreen = false;
        // but do use desktop size when going fullscreen
        bool useDesktopSize = true;
        // show the console
        bool showConsole = false;

        float deadZone = 15.0;
        float keySpeed = 75.0;
    } config;
    // where everything is drawn
    sf::RenderWindow window;
    // controls the 2D camera, used for rendering internally at a set size
    sf::View view;
    // where we render the game before copying it to the window
    sf::RenderTexture screen;
    // render internally to 720p widescreen
    unsigned int renderWidth = 1280;
    unsigned int renderHeight = 720;
    // all normal sprites to draw
    std::vector<GameSprite*> sprites;
    // the player's ship sprite
    GamePlayer* player;

    Game(const std::string&);
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
