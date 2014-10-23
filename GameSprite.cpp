#include "GameSprite.h"

GameSprite::GameSprite(string _texFile, bool _centered)
{
	texFile = _texFile;
	tex = new sf::Texture();
	if(tex->loadFromFile(texFile)) {
		tex->setSmooth(true);
		setTexture(tex, true);
		setSize((sf::Vector2f)tex->getSize());
	} else {
		setSize(sf::Vector2f(100, 100));
	}
	if(_centered) {
		setOriginAtCenter();
	}
	speed = 0.010;
}

GameSprite::GameSprite(sf::Color _color, bool _centered)
{
	setSize(sf::Vector2f(100, 100));
	setFillColor(_color);
	if(_centered) {
		setOriginAtCenter();
	}
	speed = 0.010;

}

GameSprite::~GameSprite()
{
	delete tex;
}

void GameSprite::update(const int elapsed)
{
	move(dir * (speed * elapsed));
}

void GameSprite::setOriginAtCenter()
{
	setOrigin(getLocalBounds().width / 2, getLocalBounds().height / 2);
}

void GameSprite::moveBy(float x, float y)
{
	dir.x = x;
	dir.y = y;
}
