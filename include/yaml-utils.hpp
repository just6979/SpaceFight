#pragma once

#include <easylogging++.h>

#include <SFML/Graphics/Vertex.hpp>

#include <yaml-cpp/yaml.h>

std::string nodeTypeToString(const YAML::Node& node);
sf::Vertex nodeToVertex(const YAML::Node& node, int size = 50);
sf::Color nodeToColor(const YAML::Node& node);

