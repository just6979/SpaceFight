#include <GameSprite.h>

GameSprite::GameSprite() {
}

GameSprite::GameSprite(const sf::Texture& _texture) {
    setTexture(_texture);
}

GameSprite::GameSprite(const sf::Image& _image) {
    setTexture(_image);
}

GameSprite::~GameSprite() {
}

void GameSprite::setTexture(const sf::Texture& _texture) {
    texture = _texture;
    setVertices();
}

void GameSprite::setTexture(const sf::Image& _image) {
    texture.loadFromImage(_image);
    setVertices();
}

void GameSprite::setVertices() {
    sf::Vector2u size = texture.getSize();

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(4);

    vertices[0].position = sf::Vector2f(-size.x / 2, -size.y / 2);
    vertices[1].position = sf::Vector2f(size.x / 2, -size.y / 2);
    vertices[2].position = sf::Vector2f(size.x / 2, size.y / 2);
    vertices[3].position = sf::Vector2f(-size.x / 2, size.y / 2);

    vertices[0].texCoords = sf::Vector2f(0, 0);
    vertices[1].texCoords = sf::Vector2f(size.x, 0);
    vertices[2].texCoords = sf::Vector2f(size.x, size.y);
    vertices[3].texCoords = sf::Vector2f(0, size.y);
}

void GameSprite::update(const int elapsed) {
    move(dir * (speed * elapsed));
}

void GameSprite::moveBy(const float x, const float y) {
    dir.x = x;
    dir.y = y;
}

void GameSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &texture;
    target.draw(vertices, states);
}
