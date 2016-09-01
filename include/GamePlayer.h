#pragma once

#include <GameSprite.h>

class GamePlayer : public GameSprite {
public:
    GamePlayer();
    ~GamePlayer();
private:
    unsigned int size;
    sf::Color color;
};
