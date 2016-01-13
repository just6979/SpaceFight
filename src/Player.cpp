#include <Player.h>

Player::Player(int _size, sf::Color _color) {
    size = _size;
    color = _color;
    speed = 0.010;

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(4);
    vertices[0].position = sf::Vector2f(-size/2, -size/2);
    vertices[1].position = sf::Vector2f(size/2, -size/2);
    vertices[2].position = sf::Vector2f(size/2, size/2);
    vertices[3].position = sf::Vector2f(-size/2, size/2);

    for (int i = 0; i < 4; i++) {
        vertices[i].color = color;
    }
}

Player::~Player() {

}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    //states.texture = &texture;
    target.draw(vertices, states);
}

void Player::update(const int elapsed) {
    move(dir * (speed * elapsed));
}

void Player::moveBy(float x, float y) {
    dir.x = x;
    dir.y = y;
}
