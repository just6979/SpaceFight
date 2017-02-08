
#include <utils.hpp>

std::string NodeTypeToString(const YAML::Node& node) {
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
