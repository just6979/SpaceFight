/*
	Copyright 2014 Justin White
	See included LICENSE file for details.
*/

#include <algorithm>
#include <fstream>
#include <iostream>

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

#include "GameSprite.h"

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
float xScale, yScale = 0;
sf::VideoMode desktop;
bool fullscreen = false;
const string title = "SpaceFight";
sf::RenderWindow* window;
sf::RenderTexture* screen;
sf::Sprite* screenSprite;
string fileLogName = title + ".log";
log4cpp::Category& alog = log4cpp::Category::getRoot();
log4cpp::Category& clog = log4cpp::Category::getInstance("cons");
log4cpp::Category& flog = log4cpp::Category::getInstance("file");

// game data
const float deadZone = 15;
const float keySpeed = 75;
GameSprite* player;


// system functions
void initializeSystem();
void resizeWindow(bool fullscreen);
void adjustScale();
void processEvents();
void updateControls();
void updateWorld(sf::Time elapsed);
void renderWorld();


void initializeSystem()
{
	desktop = sf::VideoMode::getDesktopMode();
	window = new sf::RenderWindow();

	screen = new sf::RenderTexture;
	screen->create(width, height, false);
	screenSprite = new sf::Sprite(screen->getTexture());

	resizeWindow(fullscreen);

	sf::ContextSettings settings = window->getSettings();
	alog.info("Using OpenGL v%d.%d", settings.majorVersion, settings.minorVersion);

	player = new GameSprite(sf::Color::Blue);
	player->setPosition(width / 2, height / 2);
	alog.info("Loaded sprite");
}

void resizeWindow(bool go_fullscreen)
{
	int flags = 0;
	sf::VideoMode mode;

	if(go_fullscreen) {
		mode = desktop;
		flags = sf::Style::Fullscreen;
	} else {
		mode = sf::VideoMode(width, height);
		flags = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
	}

	window->create(mode, title, flags);

	if(!window->isOpen()) {
		alog.error("Could not create main window");
		exit(EXIT_FAILURE);
	}

	fullscreen = go_fullscreen;
	window->setMouseCursorVisible(!fullscreen);

	if (!go_fullscreen) {
		// center the window
		window->setPosition(
			sf::Vector2i(
				desktop.width / 2 - window->getSize().x / 2,
				desktop.height / 2 - window->getSize().y / 2 - 36
			)
		);
	}
	alog.info("Created the main window %dx%d", window->getSize().x, window->getSize().y);
	window->setVerticalSyncEnabled(true);
	alog.info("Enabled VSync");
	adjustScale();
}

void adjustScale() {
	xScale = (float)window->getSize().x / width;
	yScale = (float)window->getSize().y / height;

	screenSprite->setScale(xScale, yScale);
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
		case sf::Event::Resized:
			adjustScale();
		case sf::Event::KeyPressed:
			switch(event.key.code) {
			case sf::Keyboard::Escape:
				alog.info("Player exited");
				window->close();
				break;
			case sf::Keyboard::Return:
				if(event.key.alt) {
					resizeWindow(!fullscreen);
				}
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
	float x, y = 0;

	float joy0_X = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
	float joy0_y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
	x = abs(joy0_X) < deadZone ? 0 : joy0_X;
	y = abs(joy0_y) < deadZone ? 0 : joy0_y;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		y += -keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		x += keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		y += keySpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		x += -keySpeed;
	}
	player->moveBy(x, y);
}

void updateWorld(sf::Time elapsed)
{
	const int millis = elapsed.asMilliseconds();
	player->update(millis);
}

void renderWorld()
{
	screen->clear(sf::Color::Black);
	screen->draw(*player);
	screen->display();

	window->clear(sf::Color(128,128,128));
	window->draw(*screenSprite);
	window->display();
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
