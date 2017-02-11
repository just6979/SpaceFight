#include <Sprite.hpp>

Sprite::Sprite() {
}

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

        LOG(INFO) << "Type: " << type;
        if (type == "sprite") {
            int size = dataFile["size"].as<int>(0);
            LOG(INFO) << "Size: " << size;

            vertices.setPrimitiveType(sf::Quads);

            YAML::Node newVertices = dataFile["vertices"];
            if (newVertices) {
                LOG(INFO) << "Vertices: " << newVertices.size();
                for (uint32_t i = 0; i < newVertices.size(); i++) {
                    float x = newVertices[i][0].as<float>() * size / 2;
                    float y = newVertices[i][1].as<float>() * size / 2;
                    vertices.append(sf::Vertex(sf::Vector2f(x, y)));
                    LOG(INFO) << "(" << x << ", " << y << ")";
                }
            }

            YAML::Node colors = dataFile["colors"];
            if (colors) {
                LOG(INFO) << "Colors: " << colors.size();
                for (uint32_t i = 0; i < colors.size(); i++) {
                    YAML::Node color = colors[i];
                    auto r = color[0].as<int32_t>();
                    auto g = color[1].as<uint32_t>();
                    auto b = color[2].as<uint32_t>();
                    uint32_t a = 255;
                    if (color.size() == 4) {
                        a = color[3].as<uint32_t>();
                    }
                    vertices[i].color = sf::Color(
                            static_cast<uint8_t>(r),
                            static_cast<uint8_t>(g),
                            static_cast<uint8_t>(b),
                            static_cast<uint8_t>(a)

                    );
                    LOG(INFO) << "(" << r << ", " << g << ", " << b << ")";
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
