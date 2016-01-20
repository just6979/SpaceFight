#include <Game.h>

Game& Game::getGame() {
    static Game instance;
    return instance;
}

bool Game::init(const std::string& _name) {
    if (initialized) { return true; }

    sf::ContextSettings settings = window.getSettings();
    LOG(INFO) << "Using OpenGL v" << settings.majorVersion << "." << settings.minorVersion;

    config.name = _name;

    std::string iniFilename = config.name;
    iniFilename.append(".ini");

    INIReader reader(iniFilename);
    LOG(INFO) << "Reading config from '" << iniFilename << "'";
    if (reader.ParseError() < 0) {
        LOG(ERROR) << "Can't load '" << iniFilename << "', using defaults";
    } else {
        // 1200x675 is a 16:9 window that fits inside a 1366x768 screen on most systems
        config.width = (uint32_t) abs(reader.GetInteger("game", "width", static_cast<long>(config.width)));
        config.height = (uint32_t) abs(reader.GetInteger("game", "height", static_cast<long>(config.height)));
        config.fullscreen = reader.GetBoolean("game", "fullscreen", config.fullscreen);
        config.useDesktopSize = reader.GetBoolean("game", "useDesktopSize", config.useDesktopSize);
        config.deadZone = reader.GetReal("game", "deadZone", config.deadZone);
        config.keySpeed = reader.GetReal("game", "keySpeed", config.keySpeed);
    }
    LOG(INFO) << "--Config--";
    LOG(INFO) << "width = " << config.width;
    LOG(INFO) << "height = " << config.height;
    LOG(INFO) << "fullscreen = " << (config.fullscreen ? "true" : "false");
    LOG(INFO) << "useDesktopSize = " << (config.useDesktopSize ? "true" : "False");
    LOG(INFO) << "deadZone = " << config.deadZone;
    LOG(INFO) << "keySpeed = " << config.keySpeed;
    LOG(INFO) << "--End config--";

    LOG(INFO) << "Creating " << renderWidth << "x" << renderHeight << " render target";
    screen.create(renderWidth, renderHeight);

    console.init();

    createWindow(config.fullscreen);

    player.setPosition(renderWidth * 1 / 2, renderHeight * 3 / 4);
    sprites.push_back(player);
    LOG(INFO) << "Loaded player";

    initialized = true;
    return initialized;
}

void Game::createWindow(bool shouldFullscreen) {
    unsigned int flags = 0;
    sf::VideoMode mode;
    config.fullscreen = shouldFullscreen;
    if (config.fullscreen) {
        LOG(INFO) << "Going fullscreen";
        window.setMouseCursorVisible(false);
        if (config.useDesktopSize) {
            LOG(INFO) << "Setting fullscreen mode (using desktop size): " << \
                sf::VideoMode::getDesktopMode().width << "x" << \
                sf::VideoMode::getDesktopMode().height;
            mode = sf::VideoMode::getDesktopMode();
        } else {
            LOG(INFO) << "Setting fullscreen mode: " << config.width << "x" << config.height;
            mode = sf::VideoMode(config.width, config.height);
        }
        flags = sf::Style::Fullscreen;
    } else {
        LOG(INFO) << "Setting windowed mode: " << config.width << "x" << config.height;
        window.setMouseCursorVisible(true);
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }
    window.create(mode, config.name, flags);
    if (!window.isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }
    // initialize the view
    view = window.getDefaultView();
    view.setSize(renderWidth, renderHeight);
    view.setCenter(renderWidth / 2, renderHeight / 2);
    window.setView(view);
    LOG(INFO) << "Enabling V-sync";
    window.setVerticalSyncEnabled(true);
    // scale the viewport to maintain good aspect
    adjustAspect(window.getSize());
    // make sure the console fits too
    console.resize(screen.getSize());
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
    LOG(INFO) << "Adjusting aspect for window size " << newSize.x << "x" << newSize.y;
    LOG(INFO) << "Setting " << isSixteenNine << " viewport (wo:" << \
        widthOffset << ", ho:" << heightOffset << "; ws:" << widthScale << ", hs:" << heightScale << ")";
    view.setViewport(sf::FloatRect(widthOffset, heightOffset, widthScale, heightScale));
    window.setView(view);
}


void Game::processEvents() {
    static sf::Event event;

    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                LOG(INFO) << "Window closed";
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
            LOG(INFO) << "GamePlayer exited";
            window.close();
            break;
        case sf::Keyboard::Return:
            if (event.key.alt) {
                createWindow(!config.fullscreen);
            }
            break;
        case sf::Keyboard::Tilde:
            config.showConsole = !config.showConsole;
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
    player.moveBy(x, y);
}

void Game::updateWorld(sf::Time elapsed) {
    const int millis = elapsed.asMilliseconds();
    player.update(millis);
}

void Game::renderWorld() {
    // blank the window to gray
    window.clear(sf::Color(128, 128, 128));
    // blank the render target
    screen.clear(sf::Color::Black);
    // render all the normal sprites
    for (const auto& sprite : sprites) {
        screen.draw(sprite);
    }
    // if active, render console last so it overlays
    if (config.showConsole) {
        screen.draw(console);
    }
    // show everything rendered so far
    screen.display();
    // copy render target to window
    window.draw(sf::Sprite(screen.getTexture()));
    // display everything
    window.display();
}


void Game::run(void) {
    sf::Clock gameClock;
    sf::Time elapsed;
    LOG(INFO) << "Starting " << config.name;
    while (window.isOpen()) {
        elapsed = gameClock.restart();
        processEvents();
        updateControls();
        updateWorld(elapsed);
        renderWorld();
    }
    LOG(INFO) << "Stopped";
}
