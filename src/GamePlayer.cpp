#include <GamePlayer.h>

GamePlayer::GamePlayer() {
    size = 100;
    color = sf::Color::Blue;

    sf::RectangleShape* tmp = new sf::RectangleShape();
    tmp->setSize(sf::Vector2f(size, size));
    tmp->setFillColor(color);

    sf::Texture* tex = new sf::Texture(*(tmp->getTexture()));

    setTexture(tex);
}

GamePlayer::~GamePlayer() {

}
