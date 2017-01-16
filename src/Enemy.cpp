#include <Enemy.h>

Enemy::Enemy() {
    size = 100;
    color = sf::Color::Red;

    sf::Image image;
    image.create(size, size, color);

    setTexture(image);
}

Enemy::~Enemy() {
}
