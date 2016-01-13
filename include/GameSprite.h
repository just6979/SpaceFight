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
    GameSprite(sf::Texture* texture);
    ~GameSprite();
    void update(const int elapsed);
    void moveBy(float x = 0, float y = 0);
private:
    int size;
    sf::Color color;
    sf::VertexArray vertices;
    sf::Vector2<float> dir;
    float speed = 0.10;
    sf::Texture* texture = NULL;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif // GAMESPRITE_H
