#include "GameSprite.h"

GameSprite::GameSprite(std::string _texFile, bool _lockToScreen, bool _originAtCenter) {
    texFile = _texFile;
    tex = new sf::Texture();
    if(tex->loadFromFile(texFile)) {
        tex->setSmooth(true);
        setTexture(tex, true);
        setSize((sf::Vector2f)tex->getSize());
    } else {
        setSize(sf::Vector2f(100, 100));
    }
    lockToScreen(_lockToScreen);
    if(_originAtCenter) {
        setOriginAtCenter();
    }
    speed = 0.010;
}

GameSprite::GameSprite(sf::Color _color, bool _lockToScreen, bool _originAtCenter) {
    setSize(sf::Vector2f(100, 100));
    setFillColor(_color);
    lockToScreen(_lockToScreen);
    if(_originAtCenter) {
        setOriginAtCenter();
    }
    speed = 0.010;

}

GameSprite::~GameSprite() {
    delete tex;
}

void GameSprite::update(const int elapsed) {
    move(dir * (speed * elapsed));
}

void GameSprite::lockToScreen(bool _lockToScreen) {
    lockedToScreen = _lockToScreen;
}

void GameSprite::setOriginAtCenter() {
    setOrigin(getLocalBounds().width / 2, getLocalBounds().height / 2);
}

void GameSprite::moveBy(float x, float y) {
//    if (x + getSize().x > screen.x) {
//        x = 0;
//    }
//    if (x + getSize().x < 0) {
//        x = 0;
//    }
    dir.x = x;
    dir.y = y;
}
