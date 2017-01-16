#include <Sprite.h>

Sprite::Sprite() {
}

Sprite::Sprite(const sf::Texture& _texture) {
    setTexture(_texture);
}

Sprite::Sprite(const sf::Image& _image) {
    setTexture(_image);
}

Sprite::~Sprite() {
}

void Sprite::setTexture(const sf::Texture& _texture) {
    texture = _texture;
    setVertices();
}

void Sprite::setTexture(const sf::Image& _image) {
    texture.loadFromImage(_image);
    setVertices();
}

void Sprite::setVertices() {
    sf::Vector2u size = texture.getSize();

    setOrigin(size.x / 2, size.y / 2);

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(4);

    vertices[0].position = sf::Vector2f(0, 0);
    vertices[1].position = sf::Vector2f(0, size.y);
    vertices[2].position = sf::Vector2f(size.x, size.y);
    vertices[3].position = sf::Vector2f(size.x, 0);

    vertices[0].texCoords = sf::Vector2f(0, 0);
    vertices[1].texCoords = sf::Vector2f(size.x, 0);
    vertices[2].texCoords = sf::Vector2f(size.x, size.y);
    vertices[3].texCoords = sf::Vector2f(0, size.y);
}

void Sprite::update(const int elapsed) {
    move(dir * (speed * elapsed));
}

void Sprite::moveBy(const float x, const float y) {
    dir.x = x;
    dir.y = y;
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &texture;
    target.draw(vertices, states);
}
