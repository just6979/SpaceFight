#pragma once

#include <sstream>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

std::string colorToString(const sf::Color& color);
std::string vertexToString(const sf::Vertex& vertex);
