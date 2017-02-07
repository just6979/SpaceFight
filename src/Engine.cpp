#include <Engine.hpp>

Engine::Engine(const int _argc, const char** _argv) :
    argc(_argc), argv(_argv)
{
    // check if a game directory is specified on the command line
    if (argc > 1 and argv[1] != nullptr) {
        game = std::string(argv[1]);
    } else {
        game = "game";
    }

    LOGOG_INITIALIZE();

    // create custom format
    class FormatterCustom : public logog::FormatterGCC {
        virtual TOPIC_FLAGS GetTopicFlags(const logog::Topic& topic) {
            return  (logog::FormatterGCC::GetTopicFlags(topic) &
                         ~(TOPIC_FILE_NAME_FLAG | TOPIC_LINE_NUMBER_FLAG));
        }
    };

    const std::string logFilename = data_dir + '/' + game + ".log";

    // remove existing log file
    remove(logFilename.c_str());

    logFile = new logog::LogFile(logFilename.c_str());
    logConsole = new logog::Cout;
    formatter = new FormatterCustom;
    // use custom format
    formatter->SetShowTimeOfDay(true);
    logFile->SetFormatter(*formatter);
    logConsole->SetFormatter(*formatter);

    // try to make sure we don't get extra crap in our console log
    std::cout.flush();

    INFO("Logging system initialized.");

    dumpSystemInfo();

    INFO("Initializing Engine with game data in '%s'", game.c_str());

    data_dir = game;

    // TODO: verify data_dir is accesible, abort if not
    // TODO: _chdir into data_dir for easier relative paths everywhere else

    readConfig();

    INFO("Creating %dx%d render target", renderWidth, renderHeight);
    screen.create(renderWidth, renderHeight);

    createWindow(config.fullscreen);

    std::unique_lock<std::mutex> spritesLock(spritesMutex);
    INFO("Creating entities");
    player = std::make_shared<Sprite>(data_dir + "/player.yaml");
    player->setPosition(renderWidth * 1 / 2, renderHeight * 3 / 4);
    sprites.push_back(player);
    INFO("Created player");
    enemy = std::make_shared<Sprite>(data_dir + "/enemy.yaml");
    enemy->setPosition(renderWidth * 1 / 2, renderHeight * 1 / 4);
    sprites.push_back(enemy);
    INFO("Created enemy");
    spritesLock.unlock();

    isReady = true;
    INFO("Initialization Complete");
}

Engine::~Engine() {
    INFO("Logging system shutting down.");

    delete(logFile);
    delete(logConsole);
    delete(formatter);

    LOGOG_SHUTDOWN();
}

bool Engine::ready() {
    if (isReady) {
        INFO("Game of '%s' is ready", config.name.c_str());
    } else {

    }
    return isReady;
}

bool Engine::run() {
    INFO("Starting a game of '%s'", config.name.c_str());

    INFO("Initializing event loop");
    sf::Int32 updateHz = 60;
    sf::Int32 updateWaitTime = 1'000'000 / updateHz;
    sf::Clock gameClock;
    sf::Time elapsedTime;
    sf::Time lastUpdateTime;
    sf::Int64 totalUpdateTime = 0;
    sf::Int64 averageUpdateTime;
    sf::Int32 updateCount = 0;

    INFO("Creating Render thread");
    running = true;
    renderThread = std::make_unique<std::thread>(&Engine::renderLoop, this);

    INFO("Starting event loop");
    while (running) {
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
        // update at approximately 60 Hz (16666us minus time taken by last update
        sf::sleep(sf::microseconds(updateWaitTime - lastUpdateTime.asMicroseconds()));
    }
    INFO("Stopped event loop");
    renderThread->join();
    INFO("Closing window");
    window.close();
    INFO("Game of '%s' ended successfully", config.name.c_str());
    return true;
}

void Engine::processEvents() {
    static sf::Event event;

    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                INFO("Window closed");
                running = false;
                break;
            case sf::Event::Resized:
                handleResize(event.size);
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

void Engine::update(const sf::Time& elapsed) {
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

    std::unique_lock<std::mutex> spritesLock(spritesMutex);

    player->moveBy(x, y);

    const int millis = elapsed.asMilliseconds();
    for (auto sprite : sprites) {
        sprite->update(millis);
    }

    spritesLock.unlock();
}

void Engine::renderLoop() {
    INFO("Initializing render loop");
    sf::Clock frameClock;
    sf::Int32 lastFrameTime;
    sf::Int32 averageFrameTime;
    sf::Int32 totalFrameTime = 0;
    sf::Int32 frameCount = 0;
    INFO("Starting render loop");
    while (running) {
        frameClock.restart();
        // blank the render target to black
        screen.clear(sf::Color::Black);
        // render all the normal sprites
        std::unique_lock<std::mutex> spritesLock(spritesMutex);
        for (const auto sprite : sprites) {
            screen.draw(*sprite);
        }
        spritesLock.unlock();
        // update the target
        screen.display();
        // lock and activate the window
        std::unique_lock<std::mutex> windowLock(windowMutex);
        if (window.setActive(true)) {
            // blank the window to gray
            window.clear(sf::Color(128, 128, 128));
            // copy render target to window
            window.draw(sf::Sprite(screen.getTexture()));
            // update the window
            window.display();
        } else {
            ("Failed to get window context for rendering.");
        }
        // release the lock
        windowLock.unlock();
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

void Engine::handleResize(const sf::Event::SizeEvent& newSize) {
    // save the new window size since this came from a resize event
    // not from a window creation event (initialization or fullscreen toggle)
    config.width = newSize.width;
    config.height = newSize.height;
    INFO("Window resized to: %dx%d", config.width, config.height);
    // do the calculation
    adjustAspect(sf::Vector2u(newSize.width, newSize.height));
}

void Engine::handleKeyPress(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Escape:
            INFO("Key: Escape: exiting");
            running = false;
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

void Engine::handleKeyRelease(const sf::Event& event) {
    switch (event.key.code) {
        default:
            break;
    }
}


void Engine::dumpSystemInfo() {
#ifndef NDEBUG
    INFO(argv[0]);
#endif
    // dump our own version and build info
    INFO("JAGE %d.%d.%d", JAGE_VERSION_MAJOR, JAGE_VERSION_MINOR, JAGE_VERSION_REVISION);
    INFO("Built %s %s", __DATE__, __TIME__);

    // and SFML's info
    INFO("SFML %d.%d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH);

// what compiler did we use?
#ifdef __MINGW32__
#ifdef __MINGW64__
    INFO("MinGW-w64 %s", __MINGW64_VERSION_STR);
#else
    INFO("MinGW %s", __MINGW64_VERSION_STR);
#endif
#endif

#ifdef __clang__
    INFO("CLang %s", __clang_version__);
#endif

#ifdef __GNUG__
    INFO("GCC %s", __VERSION__);
#endif

#ifdef MSC_VER
    INFO("Visual C++ %s", _MCS_VER);
#endif
}

void Engine::readConfig() {
    std::string configFilename = data_dir + "/config.yaml";
    INFO("Reading config from '%s'", configFilename.c_str());
    try {
        YAML::Node yamlConfig = YAML::LoadFile(configFilename);
        config.name = yamlConfig["name"].as<std::string>(game);
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
    INFO("\tname = %s", config.name.c_str());
    INFO("\twidth = %d", config.width);
    INFO("\theight = %d", config.height);
    INFO("\tfullscreen = %s", (config.fullscreen ? "true" : "false"));
    INFO("\tuseDesktopSize = %s", (config.useDesktopSize ? "true" : "false"));
    INFO("\tvsync = %s", (config.vsync ? "true" : "false"));
    INFO("\tdeadZone = %f", config.deadZone);
    INFO("\tkeySpeed = %f", config.keySpeed);
}

void Engine::createWindow(const bool shouldFullscreen) {
    unsigned int flags = 0;

    std::unique_lock<std::mutex> windowLock(windowMutex);

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
    window.create(mode, game, flags);
    if (!window.isOpen()) {
        ERR("Could not create main window");
        exit(EXIT_FAILURE);
    }
    window.setActive(true);
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
    } else {
        window.setVerticalSyncEnabled(false);
    }

    window.setActive(false);
    windowLock.unlock();

    // scale the viewport to maintain good aspect
    adjustAspect(window.getSize());
}

void Engine::adjustAspect(const sf::Vector2u& newSize) {
    INFO("Adjusting aspect for window size %dx%d", newSize.x, newSize.y);
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
    std::unique_lock<std::mutex> windowLock(windowMutex);
    INFO("Setting %s viewport (wo:%f, ho:%f; ws:%f, hs: %f", isSixteenNine.c_str(),
         widthOffset, heightOffset, widthScale, heightScale);
    view.setViewport(sf::FloatRect(widthOffset, heightOffset, widthScale, heightScale));
    window.setView(view);
    windowLock.unlock();
}

