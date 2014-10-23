/*
	Copyright 2014 Justin White
	See included LICENSE file for details.
*/

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cmath>

using std::string;

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
sf::RenderWindow* window;
string fileLogName = title + ".log";
log4cpp::Category& alog = log4cpp::Category::getRoot();
log4cpp::Category& clog = log4cpp::Category::getInstance("cons");
log4cpp::Category& flog = log4cpp::Category::getInstance("file");

// game data
const float logoSpeed = 0.010;
const float deadZone = 15;
const float keySpeed = 75;
sf::Sprite* logo;
sf::Vector2<float> logoVector;


// system functions
void initializeSystem();
void processEvents();
void updateControls();
void updateWorld(sf::Time);
void renderWorld();

// utility functions
void setOriginCenter(sf::Sprite*);


void initializeSystem()
{
	window = new sf::RenderWindow(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close);
	if (window->isOpen()) {
		sf::ContextSettings settings = window->getSettings();
		alog.info("Using OpenGL v%d.%d",settings.majorVersion, settings.minorVersion);
		alog.info("Created the main window %dx%d", width, height);
	} else {
		alog.error("Could not create main window");
		exit(EXIT_FAILURE);
	}
	alog.info("Enabling VSync");
	window->setVerticalSyncEnabled(true);

	sf::Texture* logoTexture = new sf::Texture();
	if(!logoTexture->loadFromFile("cb.bmp")) {
		exit(EXIT_FAILURE);
	}
	alog.info("Loaded sprite");
	logoTexture->setSmooth(true);
	logo = new sf::Sprite();
	logo->setTexture(*logoTexture, true);
	setOriginCenter(logo);
	logo->move(width / 2, height / 2);
}

void processEvents()
{
	static sf::Event event;

	while(window->pollEvent(event)) {
		switch(event.type) {
		case sf::Event::Closed:
			alog.info("Window closed");
			window->close();
			break;
		case sf::Event::KeyPressed:
			switch(event.key.code) {
			case sf::Keyboard::Escape:
				alog.info("Player exited");
				window->close();
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
	logo->move(logoVector * (logoSpeed * millis));
}

void renderWorld()
{
	window->clear(sf::Color::Black);
	window->draw(*logo);
	window->display();
}


void setOriginCenter(sf::Sprite* sprite)
{
	sprite->setOrigin(sprite->getLocalBounds().width / 2, sprite->getLocalBounds().height / 2);
}


int main()
{
	log4cpp::Category::setRootPriority(DEBUG);

	// console log: 'time priority message'
	log4cpp::PatternLayout* consLogLayout = new log4cpp::PatternLayout();
	consLogLayout->setConversionPattern("%d{%H:%M:%S,%l} %p %m%n");
	log4cpp::Appender* consLog = new log4cpp::OstreamAppender("cons", &std::clog);
	consLog->setLayout(consLogLayout);
	// show debug priority & above when debugging
#ifdef DO_DEBUG
	consLog->setThreshold(DEBUG);
#else
	// show only info priority & above normally
	consLog->setThreshold(INFO);
#endif

	// file log: 'date time priority message'
	log4cpp::PatternLayout* fileLogLayout = new log4cpp::PatternLayout();
	fileLogLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S,%l} %p %m%n");
	log4cpp::Appender* fileLog = new log4cpp::FileAppender("file", fileLogName, false);
	fileLog->setLayout(fileLogLayout);
	// always show debug priority & above
	fileLog->setThreshold(DEBUG);

	alog.addAppender(consLog);
	alog.addAppender(fileLog);

	clog.addAppender(consLog);
	clog.setAdditivity(false);
	flog.addAppender(fileLog);
	flog.setAdditivity(false);

	flog << DEBUG << "Opened logFile " << fileLogName;

	alog.info("Spacefight v%d.%d", version, revision);
	alog.info("Built %s %s", __DATE__, __TIME__);
	alog.info("GCC %s", __VERSION__);
	alog.info("SFML %d.%d", SFML_VERSION_MAJOR, SFML_VERSION_MINOR);
	initializeSystem();
	sf::Clock gameClock;
	sf::Time elapsed;
	alog.info("Running...");
	while(window->isOpen()) {
		elapsed = gameClock.restart();
		processEvents();
		updateControls();
		updateWorld(elapsed);
		renderWorld();
	}
	alog.info("Stopped");

	flog << DEBUG << "Closed logFile " << fileLogName;
	fileLog->close();

	return EXIT_SUCCESS;
}
