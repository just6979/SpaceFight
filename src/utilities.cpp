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
    auto r = node[0].as<uint32_t>(255);
    auto g = node[1].as<uint32_t>(255);
    auto b = node[2].as<uint32_t>(255);
    auto a = node[3].as<uint32_t>(255);

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

    return sf::Vertex(sf::Vector2f(x, y));
}
