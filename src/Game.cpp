#include <Game.h>

Game& Game::getGame(const std::string& _name) {
    static Game* instance = new Game(_name);
    INFO("Getting Game instance");
    return *instance;
}

Game::Game(const std::string& _name) {
    INFO("Initializing new Game: %s", _name.c_str());
    config.name = _name;

    std::string iniFilename = config.name;
    iniFilename.append(".ini");

    INIReader reader(iniFilename);
    INFO("Reading config from '%s'", iniFilename.c_str());
    if (reader.ParseError() < 0) {
        ERR("Can't load '%s', using defaults", iniFilename.c_str());
    } else {
        // 1200x675 is a 16:9 window that fits inside a 1366x768 screen on most systems
        config.width = (unsigned int) std::abs(reader.GetInteger("game", "width", (long) config.width));
        config.height = (unsigned int) std::abs(reader.GetInteger("game", "height", (long) config.height));
        config.fullscreen = reader.GetBoolean("game", "fullscreen", config.fullscreen);
        config.useDesktopSize = reader.GetBoolean("game", "useDesktopSize", config.useDesktopSize);
        config.deadZone = static_cast<float>(reader.GetReal("game", "deadZone", config.deadZone));
        config.keySpeed = static_cast<float>(reader.GetReal("game", "keySpeed", config.keySpeed));
    }
    INFO("--Config--");
    INFO("width = %d", config.width);
    INFO("height = %d", config.height);
    INFO("fullscreen = %s", (config.fullscreen ? "true" : "false"));
    INFO("useDesktopSize = %s", (config.useDesktopSize ? "true" : "false"));
    INFO("deadZone = %f", config.deadZone);
    INFO("keySpeed = %f", config.keySpeed);
    INFO("--End config--");

    INFO("Creating %dx%d render target", renderWidth, renderHeight);
    screen.create(renderWidth, renderHeight);

    createWindow(config.fullscreen);

    spritesMutex.lock();
    player = new GamePlayer;
    player->setPosition(renderWidth * 1 / 2, renderHeight * 3 / 4);
    sprites.push_back(player);
    spritesMutex.unlock();
    INFO("Loaded player");

    isReady = true;
    INFO("Initialized");
}

bool Game::ready() {
    return isReady;
}

void Game::createWindow(bool shouldFullscreen) {
    unsigned int flags = 0;
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
    windowMutex.lock();
    window.create(mode, config.name, flags);
    if (!window.isOpen()) {
        ERR("Could not create main window");
        exit(EXIT_FAILURE);
    }
    sf::ContextSettings settings = window.getSettings();
    INFO("Using OpenGL %d.%d", settings.majorVersion, settings.minorVersion);

    // initialize the view
    view = window.getDefaultView();
    view.setSize(renderWidth, renderHeight);
    view.setCenter(renderWidth / 2, renderHeight / 2);
    window.setView(view);
    INFO("Enabling V-sync");
    window.setVerticalSyncEnabled(true);
    // scale the viewport to maintain good aspect
    adjustAspect(window.getSize());
    windowMutex.unlock();
}

void Game::adjustAspect(sf::Event::SizeEvent newSize) {
    // save the new window size since this came from a resize event
    // not from initialization or a fullscreen toggle
    config.width = newSize.width;
    config.height = newSize.height;
    // do the calculation
    adjustAspect(sf::Vector2u(newSize.width, newSize.height));
}

void Game::adjustAspect(sf::Vector2u newSize) {
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
    INFO("Adjusting aspect for window size ", newSize.x, newSize.y);
    INFO("Setting %s viewport (wo:%f, ho:%f; ws:%f, hs: %f", isSixteenNine.c_str(),
         widthOffset, heightOffset, widthScale, heightScale);
    view.setViewport(sf::FloatRect(widthOffset, heightOffset, widthScale, heightScale));
    windowMutex.lock();
    window.setView(view);
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
            INFO("GamePlayer exited");
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

void Game::handleKeyRelease(const sf::Event& event) const {
    switch (event.key.code) {
        default:
            break;
    }
}

void Game::updateControls() {
    float x, y = 0;

    float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    x = fabs(joy0_X) < config.deadZone ? 0 : joy0_X;
    y = fabs(joy0_y) < config.deadZone ? 0 : joy0_y;

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
    spritesMutex.unlock();

}

void Game::updateWorld(sf::Time elapsed) {
    spritesMutex.lock();
    const int millis = elapsed.asMilliseconds();
    for (auto sprite : sprites) {
        sprite->update(millis);
    }
    spritesMutex.unlock();
}


void Game::run(void) {
    window.setActive(false);
    sf::Thread renderThread(&Game::renderLoop, this);
    renderThread.launch();

    INFO("Initializing eventLoop");
    sf::Clock gameClock;
    sf::Time elapsedTime;
    sf::Int32 lastUpdateTime;
    sf::Int32 totalUpdateTime = 0;
    sf::Int32 averageUpdateTime;
    sf::Int32 updateCount = 0;
    INFO("Starting eventLoop");
    while (window.isOpen()) {
        elapsedTime = gameClock.restart();
        processEvents();
        updateControls();
        updateWorld(elapsedTime);
        lastUpdateTime = gameClock.getElapsedTime().asMilliseconds();
        totalUpdateTime += lastUpdateTime;
        averageUpdateTime = totalUpdateTime / ++updateCount;
        if (updateCount % 100 == 0) {
            DBUG("Average update time: %d ms", averageUpdateTime);
        }
        sf::sleep(sf::milliseconds(16));
    }
    INFO("Stopped eventLoop");
}

void Game::renderLoop(void) {
    INFO("Initializing renderLoop");
    sf::Clock frameClock;
    sf::Int32 lastFrameTime;
    sf::Int32 averageFrameTime;
    sf::Int32 totalFrameTime = 0;
    sf::Int32 frameCount = 0;
    window.setActive(true);
    INFO("Starting renderLoop");
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
        windowMutex.lock();
        // blank the window to gray
        window.clear(sf::Color(128, 128, 128));
        // copy render target to window
        window.draw(sf::Sprite(screen.getTexture()));
        // update thw window
        window.display();
        windowMutex.unlock();
        lastFrameTime = frameClock.getElapsedTime().asMilliseconds();
        totalFrameTime += lastFrameTime;
        averageFrameTime = totalFrameTime / ++frameCount;
        if (frameCount % 100 == 0) {
            DBUG("Average frame time: %d ms", averageFrameTime);
        }
    }
    INFO("Stopped renderLoop");
}