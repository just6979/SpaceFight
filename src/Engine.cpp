#include <Engine.hpp>

INITIALIZE_EASYLOGGINGPP

Engine::Engine(const int _argc, const char** _argv) :
        argc(_argc), argv(_argv) {
    // check if a game directory is specified on the command line
    if (argc > 1 and argv[1] != nullptr) {
        game = std::string(argv[1]);
    } else {
        game = "game";
    }

    // set some global logging flags
    el::Loggers::addFlag(el::LoggingFlag::NewLineForContainer);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::FixedTimeFormat);
    // pass command line args to elpp for configuration
    START_EASYLOGGINGPP(argc, argv);
    // load elpp configuration from file
    el::Configurations conf("logging.conf");
    // reconfigure default logger
    el::Loggers::reconfigureLogger("default", conf);

    LOG(INFO) << "Logging system initialized.";

    dumpSystemInfo();

    LOG(INFO) << "Initializing Engine with game data in '" << game << "'";

    data_dir = game;

    // TODO: verify data_dir is accessible, abort if not
    // TODO: chdir into data_dir for easier relative paths everywhere else

    readConfig();

    LOG(INFO) << "Creating " << renderWidth << " x " << renderHeight << " render target";
    screen.create(renderWidth, renderHeight);

    createWindow(config.fullscreen);

    std::unique_lock<std::mutex> spritesLock(spritesMutex);
    LOG(INFO) << "Creating entities from YAML";
    player = std::make_shared<Sprite>(data_dir + "/player.yaml");
    player->setPosition(renderWidth * 1 / 2, renderHeight * 3 / 4);
    sprites.push_back(player);
    LOG(INFO) << "Created player";
    enemy = std::make_shared<Sprite>(data_dir + "/enemy.yaml");
    enemy->setPosition(renderWidth * 1 / 2, renderHeight * 1 / 4);
    sprites.push_back(enemy);
    LOG(INFO) << "Created enemy";
    spritesLock.unlock();

    isReady = true;
    LOG(INFO) << "Initialization Complete";
}

Engine::~Engine() {
    LOG(INFO) << "Logging system shutting down";
    el::Loggers::flushAll();
}

bool Engine::ready() {
    return isReady;
}

bool Engine::run() {
    LOG(INFO) << "Starting a game: " << config.name;

    LOG(INFO) << "Creating simulation thread";
    updateThread = std::make_unique<std::thread>(&Engine::simulationThreadFunc, this);

    LOG(INFO) << "Creating render thread";
    renderThread = std::make_unique<std::thread>(&Engine::renderThreadFunc, this);

    LOG(INFO) << "Starting event loop";
    running = true;
    while (running) {
        processEvents();
        std::this_thread::yield();
    }
    LOG(INFO) << "Stopped event loop";

    LOG(INFO) << "Stopping simulation thread";
    updateThread->join();

    LOG(INFO) << "Stopping render thread";
    renderThread->join();

    if (window.isOpen()) {
        LOG(INFO) << "Closing window";
        window.close();
    }

    LOG(INFO) << "Game of '" << config.name << "' ended successfully";
    return true;
}

// runs in its own thread
void Engine::simulationThreadFunc() {
    LOG(INFO) << "Initializing simulation thread";

    uint32_t simulationWaitTime;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> startSimulationTime;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> endSimulationTime;
    std::chrono::nanoseconds lastSimulationTime = 0ns;
    std::chrono::nanoseconds elapsedSimulationTime = 0ns;
    float averageSimulationTime = 0.0f;
    uint64_t totalSimulationTime = 0;
    uint64_t simulationCycleCount = 0;

#ifndef NDEBUG
    int64_t lastLogTime = 0;
    int64_t checkLogTime = 0;
#endif

    // controller statuses
    float joy0_X, joy0_y;
    float x, y = 0;

    // compute time between updates based on desired frequency
    simulationWaitTime = static_cast<uint32_t>(1s / 1ms) / simulationHz;

    LOG(INFO) << "Simulation thread: waiting for Engine to become ready";
    while (!running) {
        std::this_thread::yield();
        std::this_thread::sleep_for(10ms);
    }

    LOG(INFO) << "Starting simulation loop";
    while (running) {
        startSimulationTime = engineClock.now();
        elapsedSimulationTime = startSimulationTime - endSimulationTime;

        //compute update time spent
        totalSimulationTime += lastSimulationTime.count();
        simulationCycleCount++;
        averageSimulationTime = static_cast<float>(totalSimulationTime) / simulationCycleCount;
#ifndef NDEBUG
        // log the average time per frame once per second
        checkLogTime = engineClock.now().time_since_epoch().count();
        if (checkLogTime - lastLogTime > (1s / 1ns)) {
            LOG(INFO) << "Average simulation time: " << averageSimulationTime / (1ms / 1ns) << "ms";
            lastLogTime = checkLogTime;
        }
#endif

        // get current state of controls
        joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
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

        player->setVelocityDir(x, y);

        for (auto sprite : sprites) {
            sprite->update(elapsedSimulationTime);
        }

        spritesLock.unlock();

        // remember how long the above code took, for updateLoop time spent calculation
        endSimulationTime = engineClock.now();
        lastSimulationTime = endSimulationTime - startSimulationTime;
        // sleep long enough to updateLoop at approximately updateHz
        sf::Int64 timeToWait = simulationWaitTime - (endSimulationTime - startSimulationTime).count();
        sf::sleep(sf::microseconds(timeToWait));
    }
    LOG(INFO) << "Stopped simulation loop";
    LOG(INFO) << "Average simulation time: " << averageSimulationTime / (1ms / 1ns) << "ms";
}

// runs in its own thread
void Engine::renderThreadFunc() {
    LOG(INFO) << "Initializing render thread";

    std::chrono::time_point<std::chrono::high_resolution_clock> frameStart;
    std::chrono::nanoseconds lastFrameTime = 0ns;
    float averageFrameTime = 0.0f;
    uint64_t totalFrameTime = 0;
    uint64_t frameCount = 0;

#ifndef NDEBUG
    int64_t lastLogTime = 0;
    int64_t checkLogTime = 0;
#endif

    LOG(INFO) << "Render thread: waiting for Engine to become ready";
    while (!running) {
        std::this_thread::yield();
        std::this_thread::sleep_for(10ms);
    }

    LOG(INFO) << "Starting render loop";
    while (running) {
        frameStart = engineClock.now();
        // compute FPS
        totalFrameTime += lastFrameTime.count();
        frameCount++;
        averageFrameTime = static_cast<float>(totalFrameTime) / frameCount;
#ifndef NDEBUG
        // log the average time per frame once per second
        checkLogTime = engineClock.now().time_since_epoch().count();
        if (checkLogTime - lastLogTime > (1s / 1ns)) {
            LOG(INFO) << "Average frame time: " << averageFrameTime / (1ms / 1ns) << "ms";
            lastLogTime = checkLogTime;
        }
#endif

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
            LOG(INFO) << "Failed to get window context for rendering.";
        }
        // release the lock
        windowLock.unlock();

        // remember how long the whole loop took, for FPS calculation
        lastFrameTime = engineClock.now() - frameStart;
    }
    LOG(INFO) << "Stopped render loop";
    LOG(INFO) << "Average frame time: " << averageFrameTime / (1ms / 1ns) << "ms";
}

void Engine::processEvents() {
    static sf::Event event;

    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                LOG(INFO) << "Window closed";
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

void Engine::handleResize(const sf::Event::SizeEvent& newSize) {
    // save the new window size since this came from a resize event
    // not from a window creation event (initialization or fullscreen toggle)
    config.width = newSize.width;
    config.height = newSize.height;
    LOG(INFO) << "Window resized to: " << config.width << " x " << config.height;
    // do the calculation
    adjustAspect(sf::Vector2u(newSize.width, newSize.height));
}

void Engine::handleKeyPress(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Escape:
            LOG(INFO) << "Key = Escape: exiting";
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


void Engine::dumpSystemInfo() const {
#ifndef NDEBUG
    LOG(INFO) << argv[0];
#endif
    // dump our own version and build info
    LOG(INFO) << "JAGE " << JAGE_VERSION_MAJOR << "." << JAGE_VERSION_MINOR << "." << JAGE_VERSION_REVISION;
    LOG(INFO) << "Built at " << __TIME__ << " on " << __DATE__;

    // and SFML's info
    LOG(INFO) << "SFML " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH;

// what compiler did we use?
#ifdef __MINGW32__
#ifdef __MINGW64__
    LOG(INFO) << "MinGW-w64 " << __MINGW64_VERSION_STR;
#else
    LOG(INFO) << "MinGW " << __MINGW64_VERSION_STR;
#endif
#endif

#ifdef __clang__
    LOG(INFO) << "CLang " << __clang_version__;
#endif

#ifdef __GNUG__
    LOG(INFO) << "GCC " << __VERSION__;
#endif

#ifdef MSC_VER
    LOG(INFO) << "Visual C++ " << _MCS_VER;
#endif
}

void Engine::readConfig() {
    std::string configFilename = data_dir + "/config.yaml";
    LOG(INFO) << "Reading config from '" << configFilename << "'";
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
        LOG(ERROR) << "YAML Exception: " << e.msg;
		LOG(ERROR) << "Can't load '" << configFilename << "', using sane defaults";
    }
    LOG(INFO) << "Current settings:";
    LOG(INFO) << "\tname = " << config.name;
    LOG(INFO) << "\twidth = " << config.width;
    LOG(INFO) << "\theight = " << config.height;
    LOG(INFO) << "\tfullscreen = " << (config.fullscreen ? "true" : "false");
    LOG(INFO) << "\tuseDesktopSize = " << (config.useDesktopSize ? "true" : "false");
    LOG(INFO) << "\tvsync = " << (config.vsync ? "true" : "false");
    LOG(INFO) << "\tdeadZone = " << config.deadZone;
    LOG(INFO) << "\tkeySpeed = " << config.keySpeed;
}

void Engine::createWindow(const bool shouldFullscreen) {
    unsigned int flags = 0;

    std::unique_lock<std::mutex> windowLock(windowMutex);

    LOG(INFO) << "Reading config";
    sf::VideoMode mode;
    config.fullscreen = shouldFullscreen;
    if (config.fullscreen) {
        LOG(INFO) << "Going fullscreen";
        window.setMouseCursorVisible(false);
        if (config.useDesktopSize) {
            LOG(INFO) << "Setting fullscreen mode (using desktop size): "
                      << sf::VideoMode::getDesktopMode().width
                      << " x "
                      << sf::VideoMode::getDesktopMode().height;
            mode = sf::VideoMode::getDesktopMode();
        } else {
            LOG(INFO) << "Setting fullscreen mode: " << config.width << " x " << config.height;
            mode = sf::VideoMode(config.width, config.height);
        }
        flags = sf::Style::Fullscreen;
    } else {
        LOG(INFO) << "Setting windowed mode: " << config.width << " x " << config.height;
        window.setMouseCursorVisible(true);
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }
    LOG(INFO) << "Creating the main window";
    window.create(mode, game, flags);
    if (!window.isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }
    window.setActive(true);
    sf::ContextSettings settings = window.getSettings();
    LOG(INFO) << "Using OpenGL " << settings.majorVersion << "." << settings.minorVersion;

    // initialize the view
    LOG(INFO) << "Setting window view";
    view = window.getDefaultView();
    view.setSize(renderWidth, renderHeight);
    view.setCenter(renderWidth / 2, renderHeight / 2);
    window.setView(view);
    if (config.vsync) {
        LOG(INFO) << "Enabling v-sync";
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
    LOG(INFO) << "Adjusting aspect for window size: " << newSize.x << " x " << newSize.y;
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
    LOG(INFO) << "Setting " << isSixteenNine << " viewport "
              << "; wo: " << widthOffset
              << ", ho: " << heightOffset
              << "; ws: " << widthScale
              << ", hs: " << heightScale;
    view.setViewport(sf::FloatRect(widthOffset, heightOffset, widthScale, heightScale));
    window.setView(view);
    windowLock.unlock();
}

