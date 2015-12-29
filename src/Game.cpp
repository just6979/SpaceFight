#include <Game.h>

Game *Game::instance = NULL;

Game *Game::getGame() {
    if (instance == NULL) {
        LOG(INFO) << "Creating Game instance";
        instance = new Game();
    }
    return instance;
}

void Game::run(void) {
    sf::Clock gameClock;
    sf::Time elapsed;
    LOG(INFO) << "Starting " << name;
    while (window->isOpen()) {
        elapsed = gameClock.restart();
        processEvents();
        updateControls();
        updateWorld(elapsed);
        renderWorld();
    }
    LOG(INFO) << "Stopped";
}

bool Game::init(const std::string &_name) {
    name = _name;

    std::string iniFilename = name;
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
    LOG(INFO) << "config.width set to: " << config.width;
    LOG(INFO) << "config.height set to: " << config.height;
    LOG(INFO) << "config.fullscreen set to: " << (config.fullscreen ? "true" : "false");
    LOG(INFO) << "config.useDesktopSize set to: " << (config.useDesktopSize ? "true" : "False");
    LOG(INFO) << "config.hideMouseFullscreen set to: " << (config.hideMouseFullscreen ? "true" : "False");
    LOG(INFO) << "deadZone = " << config.deadZone;
    LOG(INFO) << "keySpeed = " << config.keySpeed;

    window = new sf::RenderWindow();

    resizeWindow(config.fullscreen);

    sf::ContextSettings settings = window->getSettings();
    LOG(INFO) << "Using OpenGL v" << settings.majorVersion << "." << settings.minorVersion;

    player = new GameSprite(sf::Color::Blue);
    player->setPosition(config.width / 2, config.height / 2);
    LOG(INFO) << "Loaded sprite";

    return true;
}

void Game::resizeWindow(bool goFullscreen) {
    unsigned int flags = 0;
    sf::VideoMode mode;

    config.fullscreen = goFullscreen;
    if (config.hideMouseFullscreen) {
        if (config.fullscreen) {
            LOG(INFO) << "Hiding mouse cursor";
            window->setMouseCursorVisible(false);
        }
        else {
            LOG(INFO) << "Showing mouse cursor";
            window->setMouseCursorVisible(true);
        }
    }

    if (config.fullscreen) {
        LOG(INFO) << "Going fullscreen";
        if (config.useDesktopSize) {
            LOG(INFO) << "Using desktop size, ignoring width and height from config";
            mode = sf::VideoMode::getDesktopMode();
        } else {
            mode = sf::VideoMode(config.width, config.height);
        }
        flags = sf::Style::Fullscreen;
    } else {
        LOG(INFO) << "Going to windowed mode";
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }

    window->create(mode, name, flags);

    if (!window->isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }

    if (config.fullscreen) {
        LOG(INFO) << "Set " << window->getSize().x << "x" << window->getSize().y << " fullscreen mode";
    } else {
        LOG(INFO) << "Created " << window->getSize().x << "x" << window->getSize().y << " window";
    }
    LOG(INFO) << "Enabling V-sync";
    window->setVerticalSyncEnabled(true);
}

void Game::processEvents() {
    static sf::Event event;

    while (window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                LOG(INFO) << "Window closed";
                window->close();
                break;
            case sf::Event::Resized:
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        LOG(INFO) << "Player exited";
                        window->close();
                        break;
                    case sf::Keyboard::Return:
                        if (event.key.alt) {
                            resizeWindow(!config.fullscreen);
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
    window->clear(sf::Color::Black);
    window->draw(*player);
    window->display();
}
