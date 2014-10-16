#include <cstdio>

#include <SFML/Graphics.hpp>

//system data
const int width = 1280;
const int height = 720;
const char* title = "SFML";
sf::RenderWindow window;

// game data
const float speed = 0.10;
sf::Texture logoTexture;
sf::Sprite logo;
sf::Vector2<float> dir;


// system functions
void initializeSystem();
void processEvents();
void updateControls();
void updateWorld(sf::Time);
void renderWorld();

// utility functions
void setOriginCenter(sf::Sprite&);


void initializeSystem()
{
	// Create the main window
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	// Load a sprite to display
	if(!logoTexture.loadFromFile("cb.bmp")) {
		exit(EXIT_FAILURE);
	}
	logoTexture.setSmooth(true);
	logo.setTexture(logoTexture, true);
	setOriginCenter(logo);
	logo.move(width / 2, height / 2);
}

void processEvents()
{
	static sf::Event event;

	while(window.pollEvent(event)) {
		switch(event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::KeyPressed:
			switch(event.key.code) {
			case sf::Keyboard::Escape:
				window.close();
				break;
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

void updateControls()
{
	dir.x = 0;
	dir.y = 0;
	// check controls
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		dir.y += -1.0;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		dir.x += 1.0;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		dir.y += 1.0;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		dir.x += -1.0;
	}
}

void updateWorld(sf::Time elapsed)
{
	const int millis = elapsed.asMilliseconds();
	logo.move(dir * (speed * millis));
}

void renderWorld()
{
	window.clear(sf::Color::Black);
	window.draw(logo);
	window.display();
}


void setOriginCenter(sf::Sprite& sprite)
{
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}


int main()
{
	initializeSystem();
	sf::Clock gameClock;
	sf::Time elapsed;

	while(window.isOpen()) {
		elapsed = gameClock.restart();
		processEvents();
		updateControls();
		updateWorld(elapsed);
		renderWorld();
	}

	return EXIT_SUCCESS;
}
