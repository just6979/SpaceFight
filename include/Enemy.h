#pragma once

#include <Sprite.h>

class Enemy : public Sprite {
public:
    Enemy();
    ~Enemy();
private:
    unsigned int size;
    sf::Color color;
};
