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

            YAML::Node verts = dataFile["verts"];
            if (verts) {
                LOG(INFO) << "Verts: " << verts.size();
                for (uint32_t i = 0; i < verts.size(); i++) {
                    float x = verts[i][0].as<float>() * size / 2;
                    float y = verts[i][1].as<float>() * size / 2;
                    vertices.append(sf::Vertex(sf::Vector2f(x, y)));
                    LOG(INFO) << "(" << x << ", " << y << ")";
                }
            }

            YAML::Node colors = dataFile["colors"];
            if (colors) {
                LOG(INFO) << "Colors: " << colors.size();
                for (uint32_t i = 0; i < colors.size(); i++) {
                    YAML::Node color = colors[i];
                    auto r = sf::Uint8(color[0].as<int>());
                    auto g = sf::Uint8(color[1].as<int>());
                    auto b = sf::Uint8(color[2].as<int>());
                    vertices[i].color = sf::Color(r, g, b);
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
