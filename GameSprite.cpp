#include "GameSprite.h"

GameSprite::GameSprite(string _texFile, bool _centered)
{
	texFile = _texFile;
	tex = new sf::Texture();
	if(tex->loadFromFile(texFile)) {
		tex->setSmooth(true);
		this->setTexture(tex, true);
		this->setSize((sf::Vector2f)tex->getSize());
	} else {
		this->setSize(sf::Vector2f(100,100));
	}
	if(_centered) {
		setOriginAtCenter();
	}
	speed = 0.010;
}

GameSprite::GameSprite(sf::Color _color, bool _centered)
{
	this->setSize(sf::Vector2f(100,100));
	this->setFillColor(_color);
		if(_centered) {
			setOriginAtCenter();
		}
	speed = 0.010;

}

GameSprite::~GameSprite()
{
	delete tex;
}

void GameSprite::update(const int elapsed) {
	this->move(dir * (speed * elapsed));
}

void GameSprite::setOriginAtCenter()
{
	setOrigin(getLocalBounds().width / 2, getLocalBounds().height / 2);
}

void GameSprite::moveBy(float x, float y) {
	dir.x = x;
	dir.y = y;
}
