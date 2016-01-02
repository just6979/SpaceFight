#include <Game.h>

Game* Game::instance = NULL;
bool Game::initialized = false;

Game* Game::getGame() {
    if (instance == NULL) {
        LOG(INFO) << "Creating Game instance";
        instance = new Game();
    }
    return instance;
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
        config.width = (unsigned int) abs(reader.GetInteger("game", "width", config.width));
        config.height = (unsigned int) abs(reader.GetInteger("game", "height", config.height));
        config.fullscreen = reader.GetBoolean("game", "fullscreen", config.fullscreen);
        config.useDesktopSize = reader.GetBoolean("game", "useDesktopSize", config.useDesktopSize);
        config.hideMouseFullscreen = reader.GetBoolean("game", "hideMouseFullscreen", config.hideMouseFullscreen);
        config.deadZone = reader.GetReal("game", "deadZone", config.deadZone);
        config.keySpeed = reader.GetReal("game", "keySpeed", config.keySpeed);
    }
    LOG(INFO) << "--Config--";
    LOG(INFO) << "width = " << config.width;
    LOG(INFO) << "height = " << config.height;
    LOG(INFO) << "fullscreen = " << (config.fullscreen ? "true" : "false");
    LOG(INFO) << "useDesktopSize = " << (config.useDesktopSize ? "true" : "False");
    LOG(INFO) << "hideMouseFullscreen = " << (config.hideMouseFullscreen ? "true" : "False");
    LOG(INFO) << "deadZone = " << config.deadZone;
    LOG(INFO) << "keySpeed = " << config.keySpeed;
    LOG(INFO) << "--End config--";


    LOG(INFO) << "Getting console";
    console = Console::getConsole();
    console->init();

    // initialize the view
    view = window.getDefaultView();

    resizeWindow(config.width, config.height, config.fullscreen);

    player = new GameSprite(sf::Color::Blue);
    player->setPosition(config.width / 2, config.height / 2);
    sprites.push_back(player);
    LOG(INFO) << "Loaded player sprite";

    initialized = true;
    return initialized;
}

void Game::resizeWindow(unsigned int width, unsigned int height, bool shouldFullscreen) {
    config.width = width;
    config.height = height;
    unsigned int flags = 0;
    sf::VideoMode mode;

    config.fullscreen = shouldFullscreen;
    if (config.hideMouseFullscreen) {
        if (config.fullscreen) {
            LOG(INFO) << "Hiding mouse cursor";
            window.setMouseCursorVisible(false);
        }
        else {
            LOG(INFO) << "Showing mouse cursor";
            window.setMouseCursorVisible(true);
        }
    }

    if (config.fullscreen) {
        LOG(INFO) << "Going fullscreen";
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
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }

    window.create(mode, config.name, flags);

    if (!window.isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }

    LOG(INFO) << "Enabling V-sync";
    window.setVerticalSyncEnabled(true);

    adjustAspect(window.getSize().x, window.getSize().y);

    console->resize(window.getSize());
}

void Game::adjustAspect(unsigned int newWidth, unsigned int newHeight) {
    float widthScale = 1.0f;
    float widthOffset = 0.0f;
    float heightScale = 1.0f;
    float heightOffset = 0.0f;

    const float sixteenNine = 16.0f / 9.0f;
    const float nineSixteen = 9.0f / 16.0f;

    float currentRatio = (float) newWidth / (float) newHeight;

    if (currentRatio > sixteenNine) {
        widthScale = newHeight * sixteenNine / newWidth;
        widthOffset = (1.0f - widthScale) / 2.0f;
    } else if (currentRatio < sixteenNine) {
        heightScale = newWidth * nineSixteen / newHeight;
        heightOffset = (1.0f - heightScale) / 2.0f;
    }

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
                resizeWindow(event.size.width, event.size.height);
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        LOG(INFO) << "Player exited";
                        window.close();
                        break;
                    case sf::Keyboard::Return:
                        if (event.key.alt) {
                            resizeWindow(config.width, config.height, !config.fullscreen);
                        }
                    default:
                        break;
                }
                break;
            case sf::Event::KeyReleased:
                switch (event.key.code) {
                    default:
                        break;
                }
                break;
            default:
                break;
        }
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
    player->moveBy(x, y);
}

void Game::updateWorld(sf::Time elapsed) {
    const int millis = elapsed.asMilliseconds();
    player->update(millis);
}

void Game::renderWorld() {
    // blank the screen
    window.clear(sf::Color::Black);
    // draw all the normal sprites
    for (const auto& sprite : sprites) {
        window.draw(*sprite);
    }
    // draw console last so it overlays
    window.draw(*console);
    // show everything
    window.display();
}
