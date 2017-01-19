#pragma once

#include <cmath>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#ifdef DO_DEBUG
#define LOGOG_LEVEL LOGOG_LEVEL_ALL
#else
#define LOGOG_LEVEL LOGOG_LEVEL_INFO
#endif
#include <logog.hpp>

#include <yaml-cpp/yaml.h>

#include <Sprite.h>
#include <Player.h>
#include <Enemy.h>

class Game {
private:
    // private ctor for singleton
    Game(const int argc, const char** argv, const std::string& _name);
public:
    static Game& getGame(const int argc, const char** argv, const std::string& _name = "");
    bool ready();
    bool run();
private:
    void showBuildInfo();
    // get the configuration from an INI file
    void readConfig();
    // [re]create the rendering window, possibly fullscreen
    void createWindow(bool shouldFullscreen = false);
    // change the viewport to maintain 16:9 aspect ratio
    void adjustAspect(sf::Event::SizeEvent newSize);
    void adjustAspect(sf::Vector2u newSize);
    // control the window mutex and active window
    void inline lockWindow();
    void inline releaseWindow();
    // main event loop
    void processEvents();
    // event handlers
    void handleKeyPress(const sf::Event& event);
    void handleKeyRelease(const sf::Event& event);
    // update the simulation
    void updateControls();
    void updateWorld(sf::Time elapsed);
    // render everything, runs in separate thread
    void renderLoop();

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
        // obvi, use vsync
        bool vsync = true;
        // controller/keyboard settings
        float deadZone = 15.0;
        float keySpeed = 75.0;
    } config;
    // where everything is drawn
    sf::RenderWindow window;
    // controls the 2D camera, used for rendering internally at a set size
    sf::View view;
    // where we render the game before copying it to the window
    sf::RenderTexture screen;
    // mutexes for the window and sprite list
    sf::Mutex windowMutex;
    sf::Mutex spritesMutex;
    // render internally to 720p widescreen
    unsigned int renderWidth = 1280;
    unsigned int renderHeight = 720;
    // all normal sprites to draw
    std::vector<Sprite*> sprites;
    // the player's ship sprite
    Player* player;
    // computer opponent's ship sprite
    Enemy* enemy;
};
