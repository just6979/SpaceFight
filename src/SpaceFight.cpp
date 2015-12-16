/*
    Copyright 2014 Justin White
    See included LICENSE file for details.
*/

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <cmath>

using std::string;

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include "easylogging++.h"
#define _ELPP_UNICODE

#include "GameSprite.h"

_INITIALIZE_EASYLOGGINGPP

//system data
const int version = 0;
const int revision = 1;
const int width = 1280;
const int height = 720;
float xScale, yScale = 0;
sf::VideoMode desktop;
bool fullscreen = false;
const string title = "SpaceFight";
sf::RenderWindow* window;
sf::RenderTexture* screen;
sf::Sprite* screenSprite;

// game data
const float deadZone = 15;
const float keySpeed = 75;
GameSprite* player;


// system functions
void initializeSystem();
void resizeWindow(bool fullscreen);
void adjustScale();
void processEvents();
void updateControls();
void updateWorld(sf::Time elapsed);
void renderWorld();


void initializeSystem() {
    desktop = sf::VideoMode::getDesktopMode();
    window = new sf::RenderWindow();

    screen = new sf::RenderTexture;
    screen->create(width, height, false);
    screenSprite = new sf::Sprite(screen->getTexture());

    resizeWindow(fullscreen);

    sf::ContextSettings settings = window->getSettings();
    LOG(INFO) << "Using OpenGL v" << settings.majorVersion << "." << settings.minorVersion;

    player = new GameSprite(sf::Color::Blue);
    player->setPosition(width / 2, height / 2);
    LOG(INFO) << "Loaded sprite";
}

void resizeWindow(bool go_fullscreen) {
    int flags = 0;
    sf::VideoMode mode;

    if(go_fullscreen) {
        mode = desktop;
        flags = sf::Style::Fullscreen;
    } else {
        mode = sf::VideoMode(width, height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }

    window->create(mode, title, flags);

    if(!window->isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }

    fullscreen = go_fullscreen;
    window->setMouseCursorVisible(!fullscreen);

    if (!go_fullscreen) {
        // center the window
        window->setPosition(
            sf::Vector2i(
                desktop.width / 2 - window->getSize().x / 2,
                desktop.height / 2 - window->getSize().y / 2 - 36
            )
        );
    }
    LOG(INFO) << "Created " << window->getSize().x << "x" << window->getSize().y << " main window";
    window->setVerticalSyncEnabled(true);
    LOG(INFO) << "Enabled VSync";
    adjustScale();
}

void adjustScale() {
    xScale = (float)window->getSize().x / width;
    yScale = (float)window->getSize().y / height;

    screenSprite->setScale(xScale, yScale);
}

void processEvents() {
    static sf::Event event;

    while(window->pollEvent(event)) {
        switch(event.type) {
        case sf::Event::Closed:
            LOG(INFO) << "Window closed";
            window->close();
            break;
        case sf::Event::Resized:
            adjustScale();
        case sf::Event::KeyPressed:
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                LOG(INFO) << "Player exited";
                window->close();
                break;
            case sf::Keyboard::Return:
                if(event.key.alt) {
                    resizeWindow(!fullscreen);
                }
            default:
                break;
            }
            break;
        case sf::Event::KeyReleased:
            switch(event.key.code) {
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void updateControls() {
    float x, y = 0;

    float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    x = abs(joy0_X) < deadZone ? 0 : joy0_X;
    y = abs(joy0_y) < deadZone ? 0 : joy0_y;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        y += -keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x += keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        y += keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x += -keySpeed;
    }
    player->moveBy(x, y);
}

void updateWorld(sf::Time elapsed) {
    const int millis = elapsed.asMilliseconds();
    player->update(millis);
}

void renderWorld() {
    screen->clear(sf::Color::Black);
    screen->draw(*player);
    screen->display();

    window->clear(sf::Color(128, 128, 128));
    window->draw(*screenSprite);
    window->display();
}

int main(int argc, char* argv[]) {
    _START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    LOG(INFO) << "Spacefight v" << version << "." << revision;
    LOG(INFO) << "Built " << __DATE__ << __TIME__;
    LOG(INFO) << "GCC " << __VERSION__;
    LOG(INFO) << "SFML " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR;
    initializeSystem();
    sf::Clock gameClock;
    sf::Time elapsed;
    LOG(INFO) << "Running...";
    while(window->isOpen()) {
        elapsed = gameClock.restart();
        processEvents();
        updateControls();
        updateWorld(elapsed);
        renderWorld();
    }
    LOG(INFO) << "Stopped";

    return EXIT_SUCCESS;
}
