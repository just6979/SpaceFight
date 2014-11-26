#ifndef GAMESPRITE_H
#define GAMESPRITE_H

#include <string>

using std::string;

#include <SFML/Graphics.hpp>

class GameSprite : public sf::RectangleShape
{
	public:
		GameSprite(string _texFile, bool _centered = true);
		GameSprite(sf::Color _color, bool _centered = true);
		virtual ~GameSprite();
		void update(const int elapsed);
		void setOriginAtCenter();
		void moveBy(float x = 0, float y = 0);
	protected:
		string texFile;
		sf::Texture* tex;
		sf::Vector2<float> dir;
		float speed;
	private:
};

#endif // GAMESPRITE_H
