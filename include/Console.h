#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include <logog/include/logog.hpp>

const float offsetPercent = 0.01;

class Console : public sf::RectangleShape {
public:
    static Console& getConsole();
    void resize(sf::Vector2u newSize);
private:
    sf::Vector2f offset;
    sf::Vector2f size;

    Console();
public:
    Console(const Console&) = delete;
    void operator=(const Console&) = delete;
};
