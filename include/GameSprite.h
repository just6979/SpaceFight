#pragma once

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <logog/include/logog.hpp>

class GameSprite : public sf::Drawable, public sf::Transformable {
public:
    GameSprite();
    GameSprite(const sf::Texture& _texture);
    GameSprite(const sf::Image& _image);
    ~GameSprite();
    void setTexture(const sf::Texture& _texture);
    void setTexture(const sf::Image& _image);
    void update(const int elapsed);
    void moveBy(const float x = 0, const float y = 0);
private:
    sf::VertexArray vertices;
    sf::Vector2f dir;
    float speed = 0.10;
    sf::Texture texture;
    void setVertices();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
