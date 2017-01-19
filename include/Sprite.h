#pragma once

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <logog.hpp>

#include <yaml-cpp/yaml.h>

class Sprite : public sf::Drawable, public sf::Transformable {
public:
    Sprite();
    Sprite(const std::string _fileName);
    Sprite(const sf::Texture& _texture);
    Sprite(const sf::Image& _image);
    ~Sprite();
    void setTexture(const sf::Texture& _texture);
    void setTexture(const sf::Image& _image);
    void update(const int elapsed);
    void moveBy(const float x = 0, const float y = 0);
private:
    std::string fileName;
    sf::VertexArray vertices;
    sf::Vector2f dir;
    float speed = 0.01;
    sf::Texture texture;
    void setVertices();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool loadFromYAML(const std::string& _fileName);
};
