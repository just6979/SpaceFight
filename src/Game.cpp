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
    }
    LOG(INFO) << "config.width set to: " << config.width;
    LOG(INFO) << "config.height set to: " << config.height;
    LOG(INFO) << "config.fullscreen set to: " << (config.fullscreen ? "true" : "false");
    LOG(INFO) << "config.useDesktopSize set to: " << (config.useDesktopSize ? "true" : "False");

    desktop = sf::VideoMode::getDesktopMode();
    window = new sf::RenderWindow();

    screen = new sf::RenderTexture;
    screen->create(config.width, config.height, false);
    screenSprite = new sf::Sprite(screen->getTexture());

    resizeWindow(config.fullscreen);

    sf::ContextSettings settings = window->getSettings();
    LOG(INFO) << "Using OpenGL v" << settings.majorVersion << "." << settings.minorVersion;

    player = new GameSprite(sf::Color::Blue);
    player->setPosition(config.width / 2, config.height / 2);
    LOG(INFO) << "Loaded sprite";

    return true;
}

void Game::resizeWindow(bool go_fullscreen) {
    unsigned int flags = 0;
    sf::VideoMode mode;

    if (go_fullscreen) {
        LOG(INFO) << "Going fullscreen";
        if (config.useDesktopSize) {
            LOG(INFO) << "Ignoring width and height from config, using desktop size";
            mode = desktop;
        } else {
            mode = sf::VideoMode(config.width, config.height);
        }
        flags = sf::Style::Fullscreen;
    }
    else {
        LOG(INFO) << "Going to windowed mode";
        mode = sf::VideoMode(config.width, config.height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }

    window->create(mode, name, flags);

    if (!window->isOpen()) {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }

    config.fullscreen = go_fullscreen;
    window->setMouseCursorVisible(!config.fullscreen);

    if (config.fullscreen) {
        LOG(INFO) << "Set " << window->getSize().x << "x" << window->getSize().y << "fullscreen mode";
    } else {
        LOG(INFO) << "Created " << window->getSize().x << "x" << window->getSize().y << " window";
    }
    LOG(INFO) << "Enabling VSync";
    window->setVerticalSyncEnabled(true);
    adjustScale();
}

void Game::adjustScale() {
    xScale = (float) window->getSize().x / config.width;
    yScale = (float) window->getSize().y / config.height;

    screenSprite->setScale(xScale, yScale);
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
                adjustScale();
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
    x = fabs(joy0_X) < deadZone ? 0 : joy0_X;
    y = fabs(joy0_y) < deadZone ? 0 : joy0_y;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        y += -keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x += keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        y += keySpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x += -keySpeed;
    }
    player->moveBy(x, y);
}

void Game::updateWorld(sf::Time elapsed) {
    const int millis = elapsed.asMilliseconds();
    player->update(millis);
}

void Game::renderWorld() {
    screen->clear(sf::Color::Black);
    screen->draw(*player);
    screen->display();

    window->clear(sf::Color(128, 128, 128));
    window->draw(*screenSprite);
    window->display();
}
