#include <Sprite.hpp>

Sprite::Sprite(const std::string& _fileName) {
	loadFromYAML(_fileName);
}

Sprite::Sprite(const sf::Texture& _texture) {
	setTexture(_texture);
}

Sprite::Sprite(const sf::Image& _image) {
	setTexture(_image);
}

Sprite::~Sprite() = default;

void Sprite::setVelocityDir(const float& x, const float& y) {
	velocityDir.x = x;
	velocityDir.y = y;
}

void Sprite::update(const std::chrono::nanoseconds& elapsed) {
	move(velocityDir * (speed * static_cast<float>(elapsed.count()) / (1s / 1ns)));
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	if (texture) {
		states.texture = texture.get();
	}
	target.draw(vertices, states);
}


bool Sprite::loadFromYAML(const std::string& _fileName) {
	fileName = _fileName;
	try {
		LOG(INFO) << "Loading '" << fileName << "'";
		YAML::Node dataFile = YAML::LoadFile(fileName);

		const std::string& type = dataFile["type"].as<std::string>("");

		LOG(INFO) << "Entity Type: " << type;
		if (type == "sprite") {
			size = dataFile["size"].as<float>(DEFAULT_SPRITE_SIZE);
			LOG(INFO) << "Initial size: " << size;

			auto rotation = dataFile["rotation"].as<float>(0.0f);
			rotate(rotation);
			LOG(INFO) << "Initial rotation: " << rotation;

			vertices.setPrimitiveType(sf::TriangleStrip);

			// get all the vertices
			std::vector<sf::Vertex> vertexList;
			YAML::Node vertexListNode = dataFile["vertices"];
			if (vertexListNode && (vertexListNode.Type() == YAML::NodeType::Sequence)) {
				LOG(INFO) << "Vertex list size: " << vertexListNode.size();
				for (
					auto&& vertexIter = vertexListNode.begin(); vertexIter != vertexListNode.end(); vertexIter++
					) {
					const YAML::Node& node = vertexIter.operator*();
					const sf::Vertex& vertex = nodeToVertex(node, size);
					vertexList.push_back(vertex);
					LOG(DEBUG) << "Vertex found: " << YAML::Dump(node) << " = " << vertexToString(vertex);
				}
			}

			// get all the colors
			std::vector<sf::Color> colorList;
			YAML::Node colorListNode = dataFile["colors"];
			if (colorListNode && (colorListNode.Type() == YAML::NodeType::Sequence)) {
				LOG(INFO) << "Color list size: " << colorListNode.size();
				for (auto&& colorIter = colorListNode.begin(); colorIter != colorListNode.end(); colorIter++) {
					const YAML::Node& node = colorIter.operator*();
					const sf::Color& color = nodeToColor(node);
					colorList.push_back(color);
					LOG(DEBUG) << "Color found: " << YAML::Dump(node) << " = " << colorToString(color);
				}
			}

			// get the indexes into the vertex and color lists/
			YAML::Node indexList = dataFile["indexes"];
			if (indexList && (indexList.Type() == YAML::NodeType::Sequence)) {
				LOG(INFO) << "Index list size: " << indexList.size();
				sf::Color color;
				bool foundColor = false;
				for (auto&& indexIter = indexList.begin(); indexIter != indexList.end(); indexIter++) {
					if (indexIter->Type() == YAML::NodeType::Map) {
						auto colorIndex = indexIter->operator[]("color").as<uint32_t>(0);
						color = colorList[colorIndex - 1];
						LOG(INFO) << "Found color index: " << colorIndex << " = " << colorToString(color);
						foundColor = true;
						continue;
					}
					if (indexIter->Type() == YAML::NodeType::Sequence) {
						for (auto indexIter2 = indexIter->begin(); indexIter2 != indexIter->end(); indexIter2++) {
							auto vertexIndex = indexIter2->as<uint32_t>(0);
							if (vertexIndex == 0) {
								color = sf::Color::White;
							}
							sf::Vertex vertex = vertexList[vertexIndex - 1];
							LOG(INFO) << "Found vertex index: " << vertexIndex << " = " << vertexToString(vertex);
							if (foundColor) {
								vertex.color = color;
							}
							vertices.append(vertex);
						}
					}
				}
			}
		}
	} catch (YAML::Exception& e) {
		LOG(ERROR) << "YAML Exception: " << e.what();
		return false;
	}
	return true;
}

void Sprite::setTexture(const sf::Texture& _texture) {
	texture = std::make_shared<sf::Texture>(_texture);
	setVerticesFromTexture();
}

void Sprite::setTexture(const sf::Image& _image) {
	sf::Texture texture;
	texture.loadFromImage(_image);
	setTexture(texture);
}

void Sprite::setVerticesFromTexture() {
	sf::Vector2f size;
	size.x = static_cast<float>(texture->getSize().x);
	size.x = static_cast<float>(texture->getSize().y);

	setOrigin(size.x / 2, size.y / 2);

	vertices.setPrimitiveType(sf::Quads);
	vertices.resize(4);

	vertices[0].position = sf::Vector2f(0.0f, 0.0f);
	vertices[0].texCoords = sf::Vector2f(0.0f, 0.0f);
	vertices[1].position = sf::Vector2f(0.0f, size.y);
	vertices[1].texCoords = sf::Vector2f(size.x, 0.0f);
	vertices[2].position = sf::Vector2f(size.x, size.y);
	vertices[2].texCoords = sf::Vector2f(size.x, size.y);
	vertices[3].position = sf::Vector2f(size.x, 0.0f);
	vertices[3].texCoords = sf::Vector2f(0.0f, size.y);
}
