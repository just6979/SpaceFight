#pragma once

#include <Sprite.h>

class Player : public Sprite {
public:
    Player();
    ~Player();
private:
    unsigned int size;
    sf::Color color;
};
