#include <Game.h>

Game::Game(const int argc, const char** argv, const std::string& _name) {
    config.name = _name;
    INFO("Initializing new Game: %s", config.name.c_str());

    INFO("Executable: %s", argv[0]);
    for (int i = 1; i < argc; i++) {
        INFO("Arg %d: %s", i, argv[i]);
    }

    showBuildInfo();

    readConfig();

    INFO("Creating %dx%d render target", renderWidth, renderHeight);
    screen.create(renderWidth, renderHeight);

    createWindow(config.fullscreen);

    spritesMutex.lock();
    INFO("Creating gameplay entities");
    player = std::make_shared<Sprite>("data/player.yaml");
    player->setPosition(renderWidth * 1 / 2, renderHeight * 3 / 4);
    sprites.push_back(player);
    INFO("Created player");
    enemy = std::make_shared<Sprite>("data/enemy.yaml");
    enemy->setPosition(renderWidth * 1 / 2, renderHeight * 1 / 4);
    sprites.push_back(enemy);
    INFO("Created enemy");
    spritesMutex.unlock();

    isReady = true;
    INFO("Initialization Complete");
}

Game& Game::getGame(const int argc, const char** argv, const std::string& _name) {
    std::string name;
    if (_name.length() == 0) {
        name = "Game";
    } else {
        name = _name;
    }
    static Game* instance = new Game(argc, argv, name);
    return *instance;
}

bool Game::ready() {
    return isReady;
}

bool Game::run() {
    INFO("Creating Render thread");
    releaseWindow();
    sf::Thread renderThread(&Game::renderLoop, this);
    renderThread.launch();

    INFO("Initializing event loop");
    sf::Int32 updateHz = 60;
    sf::Int32 updateWaitTime = 1'000'000 / updateHz;
    sf::Clock gameClock;
    sf::Time elapsedTime;
    sf::Time lastUpdateTime;
    sf::Int64 totalUpdateTime = 0;
    sf::Int64 averageUpdateTime;
    sf::Int32 updateCount = 0;
    INFO("Starting event loop");
    while (window.isOpen()) {
        elapsedTime = gameClock.restart();
        processEvents();
        update(elapsedTime);
        //compute
        lastUpdateTime = gameClock.getElapsedTime();
        totalUpdateTime += lastUpdateTime.asMilliseconds();
        averageUpdateTime = totalUpdateTime / ++updateCount;
        // log the average time per update every 1 seconds
        if (updateCount % (60 * 1) == 0) {
            DBUG("Average update time: %d ms", averageUpdateTime);
        }
        // update at approximately 60 Hz (16666us minus time taken by last update(
        sf::sleep(sf::microseconds(updateWaitTime - lastUpdateTime.asMicroseconds()));
    }
    INFO("Stopped event loop");
    return true;
}

void Game::showBuildInfo() {
    const uint32_t majorVersion = 0;
    const uint32_t minorVersion = 4;
    const uint32_t revision = 1;

    INFO("%s %d.%d.%d %s %s", config.name.c_str(), majorVersion, minorVersion, revision, __DATE__, __TIME__);

    INFO("SFML %d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR);

// what compiler are we using? just because
#ifdef __MINGW32__
#ifdef __MINGW64__
    INFO("MinGW-w64 %d.%d", __MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR);
#else
    INFO("MinGW %d.%d", __MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION);
#endif
#endif

#ifdef __clang__
    INFO("CLang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#endif

#ifdef __GNUG__
    INFO("GCC %s", __VERSION__);
#endif

#ifdef MSC_VER
    INFO("Visual C++ %s", _MCS_VER);
#endif
}


void Game::readConfig() {
    std::string configFilename = "data/config.yaml";
    INFO("Reading config from '%s'", configFilename.c_str());
    try {
        YAML::Node yamlConfig = YAML::LoadFile(configFilename);
        config.width = yamlConfig["width"].as<uint32_t>(config.width);
        config.height = yamlConfig["height"].as<uint32_t>(config.height);
        config.fullscreen = yamlConfig["fullscreen"].as<bool>(config.fullscreen);
        config.useDesktopSize = yamlConfig["useDesktopSize"].as<bool>(config.useDesktopSize);
        config.vsync = yamlConfig["vsync"].as<bool>(config.vsync);
        config.deadZone = yamlConfig["deadzone"].as<float>(config.deadZone);
        config.keySpeed = yamlConfig["keySpeed"].as<float>(config.keySpeed);
    } catch (YAML::Exception e) {
        ERR("YAML Exception: %s", e.msg.c_str());
        ERR("Can't load '%s', using sane defaults", configFilename.c_str());
    }
    INFO("Current settings:");
    INFO("\twidth = %d", config.width);
    INFO("\theight = %d", config.height);
    INFO("\tfullscreen = %s", (config.fullscreen ? "true" : "false"));
    INFO("\tuseDesktopSize = %s", (config.useDesktopSize ? "true" : "false"));
    INFO("\tvsync = %s", (config.vsync ? "true" : "false"));
    INFO("\tdeadZone = %f", config.deadZone);
    INFO("\tkeySpeed = %f", config.keySpeed);
}

void Game::createWindow(bool shouldFullscreen) {
    unsigned int flags = 0;

    lockWindow();

    INFO("Reading config");
    sf::VideoMode mode;
    config.fullscreen = shouldFullscreen;
    if (config.fullscreen) {
        INFO("Going fullscreen");
        window.setMouseCursorVisible(false);
        if (config.useDesktopSize) {
            INFO("Setting fullscreen mode (using desktop size): %dx%d",
                 sf::VideoMode::getDesktopMode().width,
                 sf::VideoMode::getDesktopMode().height);
            mode = sf::VideoMode::getDesktopMode();
        } else {
            INFO("Setting fullscreen mode: %dx%d", config.width, config.height);
            mode = sf::VideoMode(config.width, config.height);
        }
        flags = sf::Style::Fullscreen;
    } else {
        INFO("Setting windowed mode: %dx%d", config.width, config.height);
        window.setMouseCursorVisible(true);
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }
    INFO("Creating the main window");
    window.create(mode, config.name, flags);
    if (!window.isOpen()) {
        ERR("Could not create main window");
        exit(EXIT_FAILURE);
    }
    sf::ContextSettings settings = window.getSettings();
    INFO("Using OpenGL %d.%d", settings.majorVersion, settings.minorVersion);

    // initialize the view
    INFO("Setting window view");
    view = window.getDefaultView();
    view.setSize(renderWidth, renderHeight);
    view.setCenter(renderWidth / 2, renderHeight / 2);
    window.setView(view);
    if (config.vsync) {
        INFO("Enabling V-sync");
        window.setVerticalSyncEnabled(true);
    }

    releaseWindow();

    // scale the viewport to maintain good aspect
    adjustAspect(window.getSize());
}

void Game::adjustAspect(sf::Event::SizeEvent newSize) {
    // save the new window size since this came from a resize event
    // not from a window creation event (initialization or fullscreen toggle)
    config.width = newSize.width;
    config.height = newSize.height;
    INFO("Window resized to: %dx%d", config.width, config.height);
    // do the calculation
    adjustAspect(sf::Vector2u(newSize.width, newSize.height));
}

void Game::adjustAspect(sf::Vector2u newSize) {
    INFO("Adjusting aspect for window size ", newSize.x, newSize.y);
    // compute the current aspect
    float currentRatio = (float) newSize.x / (float) newSize.y;
    // used to offset and scale the viewport to maintain 16:9 aspect
    float widthScale = 1.0f;
    float widthOffset = 0.0f;
    float heightScale = 1.0f;
    float heightOffset = 0.0f;
    // used to compare and compute aspect ratios
    // for logging
    std::string isSixteenNine = "16:9";
    if (currentRatio > 16.0f / 9.0f) {
        // we are wider
        isSixteenNine = "wide";
        widthScale = newSize.y * (16.0f / 9.0f) / newSize.x;
        widthOffset = (1.0f - widthScale) / 2.0f;
    } else if (currentRatio < 16.0f / 9.0f) {
        // we are narrower
        isSixteenNine = "narrow";
        heightScale = newSize.x * (9.0f / 16.0f) / newSize.y;
        heightOffset = (1.0f - heightScale) / 2.0f;
    }
    lockWindow();
    INFO("Setting %s viewport (wo:%f, ho:%f; ws:%f, hs: %f", isSixteenNine.c_str(),
         widthOffset, heightOffset, widthScale, heightScale);
    view.setViewport(sf::FloatRect(widthOffset, heightOffset, widthScale, heightScale));
    window.setView(view);
    releaseWindow();
}

//#define LOG_WINDOW_MUTEX_LOCKS
void inline Game::lockWindow() {
#ifdef LOG_WINDOW_MUTEX_LOCKS
    DBUG("Grabbing window lock");
#endif
    windowMutex.lock();
    window.setActive(true);
}

void inline Game::releaseWindow() {
#ifdef LOG_WINDOW_MUTEX_LOCKS
    DBUG("Releasing window lock");
#endif
    window.setActive(false);
    windowMutex.unlock();
}

void Game::processEvents() {
    static sf::Event event;

    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                INFO("Window closed");
                window.close();
                break;
            case sf::Event::Resized:
                adjustAspect(event.size);
                break;
            case sf::Event::KeyPressed:
                handleKeyPress(event);
                break;
            case sf::Event::KeyReleased:
                handleKeyRelease(event);
                break;
            default:
                break;
        }
    }
}

void Game::handleKeyPress(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Escape:
            INFO("Key: Escape: exiting");
            window.close();
            break;
        case sf::Keyboard::Return:
            if (event.key.alt) {
                createWindow(!config.fullscreen);
            }
            break;
        default:
            break;
    }
}

void Game::handleKeyRelease(const sf::Event& event) {
    switch (event.key.code) {
        default:
            break;
    }
}

void Game::update(sf::Time elapsed) {
    float x, y = 0;

    // get current state of controls
    float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    x = std::fabs(joy0_X) < config.deadZone ? 0 : joy0_X;
    y = std::fabs(joy0_y) < config.deadZone ? 0 : joy0_y;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        y += -config.keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x += config.keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        y += config.keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x += -config.keySpeed;
    }

    spritesMutex.lock();
    player->moveBy(x, y);

    const int millis = elapsed.asMilliseconds();
    for (auto sprite : sprites) {
        sprite->update(millis);
    }

    spritesMutex.unlock();
}

void Game::renderLoop() {
    INFO("Initializing render loop");
    sf::Clock frameClock;
    sf::Int32 lastFrameTime;
    sf::Int32 averageFrameTime;
    sf::Int32 totalFrameTime = 0;
    sf::Int32 frameCount = 0;
    INFO("Starting render loop");
    while (window.isOpen()) {
        frameClock.restart();
        // blank the render target to black
        screen.clear(sf::Color::Black);
        // render all the normal sprites
        spritesMutex.lock();
        for (const auto sprite : sprites) {
            screen.draw(*sprite);
        }
        spritesMutex.unlock();
        // update the target
        screen.display();
        lockWindow();
        // blank the window to gray
        window.clear(sf::Color(128, 128, 128));
        // copy render target to window
        window.draw(sf::Sprite(screen.getTexture()));
        // update thw window
        window.display();
        releaseWindow();
        lastFrameTime = frameClock.getElapsedTime().asMilliseconds();
        totalFrameTime += lastFrameTime;
        averageFrameTime = totalFrameTime / ++frameCount;
        // log the time per frame every 60 frames (every second if at 60 Hz)
        if (frameCount % (60 * 1) == 0) {
            DBUG("Average frame time: %d ms", averageFrameTime);
        }
    }
    INFO("Stopped render loop");
}
