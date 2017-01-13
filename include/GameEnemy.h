#pragma once

#include <GameSprite.h>

class GameEnemy : public GameSprite {
public:
    GameEnemy();
    ~GameEnemy();
private:
    unsigned int size;
    sf::Color color;
};
