#include <utilities.hpp>

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
    auto r = node[0].as<uint32_t>();
    auto g = node[1].as<uint32_t>();
    auto b = node[2].as<uint32_t>();
    uint32_t a = 255;

    if (node.size() == 4) {
        a = node[3].as<uint32_t>();
    }

    return sf::Color(
            static_cast<uint8_t>(r),
            static_cast<uint8_t>(g),
            static_cast<uint8_t>(b),
            static_cast<uint8_t>(a)
    );
}

sf::Vertex nodeToVertex(const YAML::Node& node, int size) {
    auto x = node[0].as<float>() * size / 2;
    auto y = node[1].as<float>() * size / 2;

    sf::Vertex vertex(sf::Vector2f(x, y));

    return vertex;
}
