#include <utilities.hpp>

std::string colorToString(const sf::Color& color) {
    std::stringstream ret;
    ret << "("
        << static_cast<uint32_t>(color.r) << ", "
        << static_cast<uint32_t>(color.g) << ", "
        << static_cast<uint32_t>(color.b) << ", "
        << static_cast<uint32_t>(color.a)
        << ")";
    return ret.str();
}

std::string vertexToString(const sf::Vertex& vertex) {
    std::stringstream ret;
    ret << "(" << vertex.position.x << ", " << vertex.position.y << ")";
    return ret.str();
}

std::string nodeTypeToString(const YAML::Node& node) {
    switch (node.Type()) {
        case YAML::NodeType::Null:
            return "Null";
        case YAML::NodeType::Scalar:
            return "Scalar";
        case YAML::NodeType::Sequence:
            return "Sequence";
        case YAML::NodeType::Map:
            return "Map";
        case YAML::NodeType::Undefined:
        default:
            return "Undefined";
    }
}

sf::Color nodeToColor(const YAML::Node& node) {
    std::vector<uint8_t> color = {255, 255, 255, 255};

    uint8_t i = 0;
    for (auto&& primary : color) {
        auto newPrimary = node[i++].as<uint32_t>(255);
        if (newPrimary > 255) {
            newPrimary = 255;
        }
        primary = static_cast<uint8_t>(newPrimary);
    }
    return sf::Color(
        color[0],
        color[1],
        color[2],
        color[3]
    );
}

sf::Vertex nodeToVertex(const YAML::Node& node, float size) {
    return sf::Vertex(sf::Vector2f(node[0].as<float>(), node[1].as<float>()) * (size / 2.0f));
}
