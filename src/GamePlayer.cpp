#include <GamePlayer.h>

GamePlayer::GamePlayer() {
    size = 100;
    color = sf::Color::Blue;

    image = new sf::Image();
    image->create(size, size, color);

    tex = new sf::Texture();
    tex->create(size, size);
    tex->loadFromImage(*image);

    setTexture(tex);

    delete(image);
    delete(tex);
}

GamePlayer::~GamePlayer() {

}
