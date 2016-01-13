#ifndef SPACEFIGHT_PLAYER_H
#define SPACEFIGHT_PLAYER_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class Player : public sf::Drawable, public sf::Transformable {
public:
    Player(int size = 100, sf::Color color = sf::Color::Blue);
    ~Player();
    void update(const int elapsed);
    void moveBy(float x = 0, float y = 0);
private:
    int size;
    sf::Color color;
    sf::VertexArray vertices;
    sf::Vector2<float> dir;
    float speed;
    //sf::Texture texture;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


#endif //SPACEFIGHT_PLAYER_H
