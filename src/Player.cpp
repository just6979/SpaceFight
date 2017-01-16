#include <Player.h>

Player::Player() {
    size = 100;
    color = sf::Color::Blue;

    sf::Image image;
    image.create(size, size, color);

    setTexture(image);
}

Player::~Player() {
}
