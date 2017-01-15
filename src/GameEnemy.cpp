#include <GameEnemy.h>

GameEnemy::GameEnemy() {
    size = 100;
    color = sf::Color::Red;

    sf::Image image;
    image.create(size, size, color);

    setTexture(image);
}

GameEnemy::~GameEnemy() {
}
