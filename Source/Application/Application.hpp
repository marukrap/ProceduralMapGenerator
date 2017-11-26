#pragma once

#include "Tilemap.hpp"
#include "../Map/Map.hpp"
#include "../Generator/Generator.hpp"
#include "../Utility/Rng.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>

#include <memory> // unique_ptr

class Application
{
public:
	Application();

	void run();

private:
	void processInput();
	void update(sf::Time dt);
	void render();

	void selectNextGenerator();
	void selectPreviousGenerator();
	void generateMap();

	void registerGenerators();

private:
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Font font;

	sf::Text text;
	sf::Text textMousePos;

	Map map;
	Rng rng;
	
	std::unique_ptr<Tilemap> tilemap;

	std::vector<std::unique_ptr<Generator>> generators;
	std::size_t currentGenerator = 0;
};
