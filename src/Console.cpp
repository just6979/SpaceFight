#include <Console.h>

Console* Console::instance = NULL;

Console* Console::getConsole() {
    if (instance == NULL) {
        LOG(INFO) << "Creating Console instance";
        instance = new Console();
    }
    return instance;
}

bool Console::init() {
    if (initialized) { return true; }

    LOG(INFO) << "Initializing console";
    setFillColor(sf::Color(128, 128, 128, 128));

    initialized = true;
    return true;
}

void Console::resize(sf::Vector2u newSize) {
    offset.x = newSize.x * offsetPercent;
    offset.y = newSize.y * offsetPercent;
    size.x = newSize.x - (offset.x * 2);
    size.y = newSize.y - (offset.y * 2);
    LOG(INFO) << "Resizing console to " << size.x << "x" << size.y;
    setSize(size);
    setPosition(offset);
}
