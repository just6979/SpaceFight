/*
	Copyright 2014 Justin White
	See included LICENSE file for details.
*/

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cmath>

using namespace std;

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"

// short names for log4cpp Priorities;
const int DEBUG = log4cpp::Priority::DEBUG;
const int INFO = log4cpp::Priority::INFO;
const int WARN = log4cpp::Priority::WARN;
const int ERROR = log4cpp::Priority::ERROR;

//system data
const int version = 0;
const int revision = 1;
const int width = 1280;
const int height = 720;
const string title = "SpaceFight";
sf::RenderWindow window;
string logFilename = title + ".log";
log4cpp::Category& logger = log4cpp::Category::getRoot();

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
	window.create(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	if (window.isOpen()) {
		sf::ContextSettings settings = window.getSettings();
		logger.info("Using OpenGL v%d.%d",settings.majorVersion, settings.minorVersion);
		logger.info("Created the main window %dx%d", width, height);
	} else {
		logger.error("Could not create main window");
		exit(EXIT_FAILURE);
	}
	logger.info("Enabling VSync");
	window.setVerticalSyncEnabled(true);
	if(!logoTexture.loadFromFile("cb.bmp")) {
		exit(EXIT_FAILURE);
	}
	logger.info("Loaded sprite");
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
			logger.info("Window closed");
			window.close();
			break;
		case sf::Event::KeyPressed:
			switch(event.key.code) {
			case sf::Keyboard::Escape:
				logger.info("Player exited");
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
	log4cpp::PatternLayout* consoleLayout = new log4cpp::PatternLayout();
	consoleLayout->setConversionPattern("%d{%H:%M:%S,%l} %p %m%n");
	log4cpp::Appender* console = new log4cpp::OstreamAppender("console", &std::cout);
	console->setLayout(consoleLayout);

	log4cpp::PatternLayout* logFileLayout = new log4cpp::PatternLayout();
	logFileLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S,%l} %p %m%n");
	log4cpp::Appender* logFile = new log4cpp::FileAppender("logFile", logFilename, false);
	logFile->setLayout(logFileLayout);

	logger.addAppender(console);
	logger.addAppender(logFile);

#ifdef DO_DEBUG
	logger.setPriority(log4cpp::Priority::DEBUG);
#else
	logger.setPriority(log4cpp::Priority::INFO);
#endif

	logger << DEBUG << "Opening log " << logFilename;

	logger.info("Spacefight v%d.%d", version, revision);
	logger.info("Built %s %s", __DATE__, __TIME__);
	logger.info("GCC %s", __VERSION__);
	logger.info("SFML %d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR);
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
	logger.info("Stopped");

	logger << DEBUG << "Closing log " << logFilename;
	logFile->close();

	return EXIT_SUCCESS;
}
