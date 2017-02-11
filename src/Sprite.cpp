#include <Sprite.hpp>

Sprite::Sprite(const std::string& _fileName) {
    loadFromYAML(_fileName);
}

Sprite::Sprite(const sf::Texture& _texture) {
    setTexture(_texture);
}

Sprite::Sprite(const sf::Image& _image) {
    setTexture(_image);
}

Sprite::~Sprite() {
}

bool Sprite::loadFromYAML(const std::string& _fileName) {
    fileName = _fileName;
    try {
        LOG(INFO) << "Loading '" << fileName << "'";
        YAML::Node dataFile = YAML::LoadFile(fileName);

        const std::string& type = dataFile["type"].as<std::string>("");

        LOG(INFO) << "Entity Type: " << type;
        if (type == "sprite") {
            size = dataFile["size"].as<uint32_t>(DEFAULT_SPRITE_SIZE);
            LOG(INFO) << "Sprite Size: " << size;

            vertices.setPrimitiveType(sf::Quads);

            YAML::Node vertexList = dataFile["vertices"];
            if (vertexList && (vertexList.Type() == YAML::NodeType::Sequence)) {
                LOG(INFO) << "Vertex List Size: " << vertexList.size();
                YAML::Node colorNode;
                bool foundColorNode = false;
                for (auto vertexIter = vertexList.begin(); vertexIter != vertexList.end(); vertexIter++) {
                    if (vertexIter->Type() == YAML::NodeType::Map) {
                        colorNode = vertexIter->operator[]("color");
                        foundColorNode = true;
                        continue;
                    }
                    sf::Vertex vertex = nodeToVertex2f(*vertexIter, size);
                    if (foundColorNode) {
                        vertex.color = nodeToColor(colorNode);
                    }
                    vertices.append(vertex);
                }
            }

            YAML::Node colorList = dataFile["colors"];
            if (colorList && (colorList.Type() == YAML::NodeType::Sequence)) {
                LOG(INFO) << "Color List Size: " << colorList.size();
                for (uint32_t i = 0; i < colorList.size(); i++) {
                    vertices[i].color = nodeToColor(colorList[i]);
                }
                texture = NULL;
            }
        }
    } catch (YAML::Exception e) {
        LOG(ERROR) << "YAML Exception: " << e.what();
        return false;
    }
    return true;
}

void Sprite::setTexture(const sf::Texture& _texture) {
    texture = std::make_shared<sf::Texture>(_texture);
    setVertices();
}

void Sprite::setTexture(const sf::Image& _image) {
    texture = std::make_shared<sf::Texture>();
    texture->loadFromImage(_image);
    setVertices();
}

void Sprite::setVertices() {
    sf::Vector2u size = texture->getSize();

    setOrigin(size.x / 2, size.y / 2);

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(4);

    vertices[0].position = sf::Vector2f(0, 0);
    vertices[1].position = sf::Vector2f(0, size.y);
    vertices[2].position = sf::Vector2f(size.x, size.y);
    vertices[3].position = sf::Vector2f(size.x, 0);

    vertices[0].texCoords = sf::Vector2f(0, 0);
    vertices[1].texCoords = sf::Vector2f(size.x, 0);
    vertices[2].texCoords = sf::Vector2f(size.x, size.y);
    vertices[3].texCoords = sf::Vector2f(0, size.y);
}

void Sprite::update(const std::chrono::nanoseconds& elapsed) {
    move(dir * (speed * static_cast<float>(elapsed.count()) / 1'000'000));
}

void Sprite::moveBy(const float& x, const float& y) {
    dir.x = x;
    dir.y = y;
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    if (texture) {
        states.texture = texture.get();
    }
    target.draw(vertices, states);
}
