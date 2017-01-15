#include <GamePlayer.h>

GamePlayer::GamePlayer() {
    size = 100;
    color = sf::Color::Blue;

    sf::Image image;
    image.create(size, size, color);

    setTexture(image);
}

GamePlayer::~GamePlayer() {
}
