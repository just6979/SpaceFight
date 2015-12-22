
#include "../contrib/easylogging++.h"

#include "Game.h"

Game* Game::instance = NULL;

Game* Game::getGame()
{
    if (instance == NULL)
    {
        LOG(INFO) << "Creating Game instance";
        instance = new Game();
    }
    return instance;
}

void Game::run(void)
{
    sf::Clock gameClock;
    sf::Time elapsed;
    LOG(INFO) << "Starting " << name;
    while(window->isOpen())
    {
        elapsed = gameClock.restart();
        processEvents();
        updateControls();
        updateWorld(elapsed);
        renderWorld();
    }
    LOG(INFO) << "Stopped";
}

void Game::init(const std::string& _name, int _width, int _height)
{
    name = _name;
    width = _width;
    height = _height;

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

void Game::resizeWindow(bool go_fullscreen)
{
    int flags = 0;
    sf::VideoMode mode;

    if(go_fullscreen)
    {
        mode = desktop;
        flags = sf::Style::Fullscreen;
    }
    else
    {
        mode = sf::VideoMode(width, height);
        flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    }

    window->create(mode, name, flags);

    if(!window->isOpen())
    {
        LOG(ERROR) << "Could not create main window";
        exit(EXIT_FAILURE);
    }

    fullscreen = go_fullscreen;
    window->setMouseCursorVisible(!fullscreen);

    if (!go_fullscreen)
    {
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

void Game::adjustScale()
{
    xScale = (float)window->getSize().x / width;
    yScale = (float)window->getSize().y / height;

    screenSprite->setScale(xScale, yScale);
}

void Game::processEvents()
{
    static sf::Event event;

    while(window->pollEvent(event))
    {
        switch(event.type)
        {
        case sf::Event::Closed:
            LOG(INFO) << "Window closed";
            window->close();
            break;
        case sf::Event::Resized:
            adjustScale();
        case sf::Event::KeyPressed:
            switch(event.key.code)
            {
            case sf::Keyboard::Escape:
                LOG(INFO) << "Player exited";
                window->close();
                break;
            case sf::Keyboard::Return:
                if(event.key.alt)
                {
                    resizeWindow(!fullscreen);
                }
            default:
                break;
            }
            break;
        case sf::Event::KeyReleased:
            switch(event.key.code)
            {
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void Game::updateControls()
{
    float x, y = 0;

    float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    x = abs(joy0_X) < deadZone ? 0 : joy0_X;
    y = abs(joy0_y) < deadZone ? 0 : joy0_y;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        y += -keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        x += keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        y += keySpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        x += -keySpeed;
    }
    player->moveBy(x, y);
}

void Game::updateWorld(sf::Time elapsed)
{
    const int millis = elapsed.asMilliseconds();
    player->update(millis);
}

void Game::renderWorld()
{
    screen->clear(sf::Color::Black);
    screen->draw(*player);
    screen->display();

    window->clear(sf::Color(128, 128, 128));
    window->draw(*screenSprite);
    window->display();
}

