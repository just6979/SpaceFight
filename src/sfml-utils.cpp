#include "sfml-utils.hpp"

std::string colorToString(const sf::Color& color) {
    std::stringstream ret;
    ret << "(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ", " << ")";
    return ret.str();
}

std::string vertexToString(const sf::Vertex& vertex) {
    std::stringstream ret;
    ret << "(" << vertex.position.x << ", " << vertex.position.y << ")";
    return ret.str();
}
