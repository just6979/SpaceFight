#pragma once

#include <chrono>

#include <easylogging++.h>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <yaml-cpp/yaml.h>

#include <utilities.hpp>

using namespace std::chrono_literals;

class Sprite : public sf::Drawable, public sf::Transformable {
public:
    const float DEFAULT_SPRITE_SIZE = 50.0f;

    Sprite() = delete;
    Sprite(const std::string& _fileName);
    Sprite(const sf::Texture& _texture);
    Sprite(const sf::Image& _image);
    ~Sprite();

    void setVelocityDir(const float& x = 0, const float& y = 0);
    void update(const std::chrono::nanoseconds& elapsed);

private:
    std::string fileName;
    float size;
    sf::VertexArray vertices;
    sf::Vector2f velocityDir;
    float speed = 10;
    std::shared_ptr<sf::Texture> texture;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    bool loadFromYAML(const std::string& _fileName);

    void setTexture(const sf::Texture& _texture);
    void setTexture(const sf::Image& _image);
    void setVerticesFromTexture();
};
