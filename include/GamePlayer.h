#ifndef SPACEFIGHT_PLAYER_H
#define SPACEFIGHT_PLAYER_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <GameSprite.h>

class GamePlayer : public GameSprite {
public:
    GamePlayer();
    ~GamePlayer();
private:
    uint32_t size;
    sf::Color color;
    sf::Image* image;
};

#endif //SPACEFIGHT_PLAYER_H
