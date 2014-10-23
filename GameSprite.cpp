#include "GameSprite.h"

GameSprite::GameSprite(string _texFile, bool _centered)
{
	texFile = _texFile;
	tex = new sf::Texture();
	if(tex->loadFromFile(texFile)) {
		tex->setSmooth(true);
		this->setTexture(*tex, true);
		if(_centered) {
			setOriginAtCenter();
		}
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
