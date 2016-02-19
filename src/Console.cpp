#include <Console.h>

Console& Console::getConsole() {
    static Console instance;
    return instance;
}

bool Console::init() {
    if (initialized) { return true; }

    LOGOG_INFO("Initializing console");
    setFillColor(sf::Color(128, 128, 128, 128));

    initialized = true;
    return true;
}

void Console::resize(sf::Vector2u newSize) {
    offset.x = newSize.x * offsetPercent;
    offset.y = newSize.y * offsetPercent;
    size.x = newSize.x - (offset.x * 2);
    size.y = newSize.y - (offset.y * 2);
    LOGOG_INFO("Resizing console to %fx%f", size.x, size.y);
    setSize(size);
    setPosition(offset);
}
