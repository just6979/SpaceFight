#ifndef SPACEFIGHT_CONSOLE_H
#define SPACEFIGHT_CONSOLE_H

#include <SFML/Graphics/RectangleShape.hpp>

#include <easylogging++.h>

const float offsetPercent = 0.01;

class Console : public sf::RectangleShape {
public:
    static Console* getConsole();
    bool init();
    void resize(sf::Vector2u newSize);
private:
    static Console* instance;
    bool initialized = false;
    sf::Vector2f offset;
    sf::Vector2f size;

    Console() { };
};

#endif //SPACEFIGHT_CONSOLE_H
