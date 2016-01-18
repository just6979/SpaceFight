#ifndef GAMESPRITE_H
#define GAMESPRITE_H

#include <string>

#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class GameSprite : public sf::Drawable, public sf::Transformable {
public:
    GameSprite();
    GameSprite(const sf::Texture* _texture);
    ~GameSprite();
    void setTexture(const sf::Texture* _texture);
    void update(const int elapsed);
    void moveBy(const float x = 0, const float y = 0);
private:
    sf::VertexArray vertices;
    sf::Vector2<float> dir;
    float speed = 0.10;
    sf::Texture* texture = NULL;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif // GAMESPRITE_H
