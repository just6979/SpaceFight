#pragma once

#include <chrono>
#include <mutex>
#include <thread>

#define ELPP_THREAD_SAFE
#define ELPP_FORCE_USE_STD_THREAD
// don't log to the default file if the config doesn't load
#define ELPP_NO_LOG_TO_FILE
// but do always make a new log file when needed
#define ELPP_FRESH_LOG_FILE
#include <easylogging++.h>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <yaml-cpp/yaml.h>

#include <Sprite.hpp>

using namespace std::chrono_literals;

class Engine {
public:
    const uint32_t JAGE_VERSION_MAJOR = 0;
    const uint32_t JAGE_VERSION_MINOR = 5;
    const uint32_t JAGE_VERSION_REVISION = 2;

    const uint32_t simulationHz = 120;

    Engine(const int argc, const char** argv);
    ~Engine();

    bool ready();

    // main event loop
    bool run();

private:
    /* methods */

    //event dispatcher
    void processEvents();
    // update the simulation
    void simulationThreadFunc();
    // render everything, runs in separate thread
    void renderThreadFunc();

    // event handlers
    void handleResize(const sf::Event::SizeEvent& newSize);
    void handleKeyPress(const sf::Event& event);
    void handleKeyRelease(const sf::Event& event);

    // log some build & system info
    void dumpSystemInfo() const;

    // get the configuration from an INI file
    void readConfig();

    // [re]create the rendering window, possibly fullscreen
    void createWindow(const bool shouldFullscreen = false);
    // change the viewport to maintain 16:9 aspect ratio
    void adjustAspect(const sf::Vector2u& newSize);

    /* member data */

    int argc;
    const char** argv;

    std::chrono::high_resolution_clock engineClock;

    std::unique_ptr<std::thread> updateThread;
    std::unique_ptr<std::thread> renderThread;

    bool isReady = false;
    bool running = false;

    std::string game;
    std::string data_dir;

    // render internally to 720p widescreen
    unsigned int renderWidth = 1280;
    unsigned int renderHeight = 720;

    struct {
        std::string name;
        // this size fits in most screens in windowed mode
        unsigned int width = 1200;
        unsigned int height = 675;
        // don't start fullscreen
        bool fullscreen = false;
        // but do use desktop size when going fullscreen
        bool useDesktopSize = true;
        bool vsync = true;
        // controller/keyboard settings
        float deadZone = 15.0;
        float keySpeed = 75.0;
    } config;

    // where everything is drawn
    sf::RenderWindow window;
    // controls the 2D camera, used for rendering internally at a set size
    sf::View view;

    // mutexes for the window and sprite list
    std::mutex windowMutex;
    std::mutex spritesMutex;

    // all normal sprites to draw
    std::vector<std::shared_ptr<Sprite>> sprites;

    // the player's ship sprite
    std::shared_ptr<Sprite> player;
    // computer opponent's ship sprite
    std::shared_ptr<Sprite> enemy;
};
