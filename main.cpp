#include <iostream>
#include <fstream>

#include <cstdio>

using namespace std;

#include <SFML/Graphics.hpp>

#include "Logger.h"

//system data
const int width = 1280;
const int height = 720;
const char* title = "SpaceFight";
sf::RenderWindow window;
Logger logger("log.txt");

// game data
const float logoSpeed = 0.010;
const float deadZone = 15;
const float keySpeed = 75;
sf::Texture logoTexture;
sf::Sprite logo;
sf::Vector2<float> logoVector;


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
	logger.info("Create the main window.");
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	logger.info("Load a sprite to display.");
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
			logger.info("Window closed.");
			window.close();
			break;
		case sf::Event::KeyPressed:
			switch(event.key.code) {
			case sf::Keyboard::Escape:
				logger.info("Player exited.");
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
	logoVector.x = 0;
	logoVector.y = 0;

	float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
	float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
	logoVector.x = abs(joy0_X) < deadZone ? 0 : joy0_X;
	logoVector.y = abs(joy0_y) < deadZone ? 0 : joy0_y;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		logoVector.y += -keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		logoVector.x += keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		logoVector.y += keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		logoVector.x += -keySpeed;
	}
}

void updateWorld(sf::Time elapsed)
{
	const int millis = elapsed.asMilliseconds();
	logo.move(logoVector * (logoSpeed * millis));
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
	logger.info("Starting up...");
	initializeSystem();
	sf::Clock gameClock;
	sf::Time elapsed;
	logger.info("Running...");
	while(window.isOpen()) {
		elapsed = gameClock.restart();
		processEvents();
		updateControls();
		updateWorld(elapsed);
		renderWorld();
	}
	logger.info("Exiting.");
	return EXIT_SUCCESS;
}
