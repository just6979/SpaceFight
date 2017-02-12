#pragma once

#include <sstream>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <yaml-cpp/yaml.h>

std::string colorToString(const sf::Color& color);
std::string vertexToString(const sf::Vertex& vertex);

std::string nodeTypeToString(const YAML::Node& node);

sf::Vertex nodeToVertex(const YAML::Node& node, float size);
sf::Color nodeToColor(const YAML::Node& node);

