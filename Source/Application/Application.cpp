#include "Application.hpp"
#include "WindowsHelper.hpp"

// Resources
#include "Resources/Tileset.cpp"
#include "Resources/Font.cpp"

// Map Generators
#include "../Generator/Forest.hpp"
#include "../Generator/Cave.hpp"
#include "../Generator/Dungeon.hpp"
#include "../Generator/Spaceship.hpp"

#include <SFML/Window/Event.hpp>

#include <iostream>

namespace
{
	const sf::Vector2i windowSize(1200, 720);
	const sf::Vector2i tileSize(16, 16);
	const sf::Vector2i mapSize(windowSize.x / tileSize.x, windowSize.y / tileSize.y); // 75, 45
}

Application::Application()
	: window(sf::VideoMode(windowSize.x, windowSize.y), "Map Generator", sf::Style::Close)
	, map(mapSize.x, mapSize.y)
{
	centerWindow(window.getSystemHandle());
	window.setFramerateLimit(60);

	// texture.loadFromFile("Textures/tiny_dungeon_world.png");
	// font.loadFromFile("Fonts/neodgm.ttf");

	texture.loadFromMemory(Tileset, TilesetLength);
	font.loadFromMemory(Font, FontLength);

	text.setFont(font);
	text.setCharacterSize(16);
	text.setOutlineThickness(1.f);
	text.setPosition(5.f, 0.f);

	textMousePos.setFont(font);
	textMousePos.setCharacterSize(16);
	textMousePos.setOutlineThickness(1.f);
	textMousePos.setPosition(5.f, font.getLineSpacing(16));

	tilemap = std::make_unique<Tilemap>(texture, tileSize);

	registerGenerators();
	generateMap();
}

void Application::run()
{
	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Time dt = clock.restart();

		processInput();
		update(dt);
		render();
	}
}

void Application::processInput()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		else if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				window.close();
				break;

			case sf::Keyboard::Space:
			case sf::Keyboard::Return:
				rng.reset();
				generateMap();
				break;

			case sf::Keyboard::Up:
			case sf::Keyboard::Left:
				selectPreviousGenerator();
				break;

			case sf::Keyboard::Down:
			case sf::Keyboard::Right:
				selectNextGenerator();
				break;
			}
		}

		else if (event.type == sf::Event::MouseMoved)
		{
			int x = event.mouseMove.x / tileSize.x;
			int y = event.mouseMove.y / tileSize.y;
			
			if (map.isInBounds(x, y))
			{
				std::wstring str;

				switch (map.getTile(x, y))
				{
				case Tile::Unused:		str = L"Unused"; break;
				case Tile::Floor:		str = L"Floor"; break;
				case Tile::Corridor:	str = L"Corridor"; break;
				case Tile::Wall:		str = L"Wall"; break;
				case Tile::ClosedDoor:	str = L"Closed Door"; break;
				case Tile::OpenDoor:	str = L"Open Door"; break;
				case Tile::UpStairs:	str = L"Up Stairs"; break;
				case Tile::DownStairs:	str = L"Down Stairs"; break;
				case Tile::Grass:		str = L"Grass"; break;
				case Tile::Tree:		str = L"Tree"; break;
				case Tile::Water:		str = L"Water"; break;
				case Tile::Bridge:		str = L"Bridge"; break;
				case Tile::Dirt:		str = L"Dirt"; break;
				case Tile::CaveWall:	str = L"Cave Wall"; break;
				case Tile::Lava:		str = L"Lava"; break;
				// case Tile::Pit:			str = L"Pit"; break;
				case Tile::Void:		str = L"Void"; break;
				case Tile::VoidWall:	str = L"Void Wall"; break;
				default:				str = L"Unknown"; break;
				}

				textMousePos.setString(std::to_wstring(x) + L',' + std::to_wstring(y) + L' ' + str);
			}
		}

		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				rng.reset();
				generateMap();
			}
			
			/*
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				int x = event.mouseButton.x / tileSize.x;
				int y = event.mouseButton.y / tileSize.y;
				
				if (map.getTile(x, y) == Tile::Floor)
					map.setTile(x, y, Tile::Wall);
				else if (map.getTile(x, y) == Tile::Wall)
					map.setTile(x, y, Tile::Floor);

				tilemap->load(map);
			}
			*/
		}

		else if (event.type == sf::Event::MouseWheelScrolled)
		{
			if (event.mouseWheelScroll.delta > 0) // scroll up
				selectPreviousGenerator();
			else if (event.mouseWheelScroll.delta < 0) // scroll down
				selectNextGenerator();
		}
	}
}

void Application::update(sf::Time dt)
{
}

void Application::render()
{
	window.clear();
	window.draw(*tilemap);
	window.draw(text);
	window.draw(textMousePos);
	window.display();
}

void Application::selectNextGenerator()
{
	currentGenerator = (currentGenerator + 1) % generators.size();
	rng.reset();
	generateMap();
}

void Application::selectPreviousGenerator()
{
	currentGenerator = (currentGenerator + generators.size() - 1) % generators.size();
	rng.reset();
	generateMap();
}

void Application::generateMap()
{
	sf::Clock clock;
	generators[currentGenerator]->generate(map, rng);
	sf::Time elapsedTime = clock.getElapsedTime();

	text.setString(generators[currentGenerator]->getName()
#ifdef _DEBUG
		+ L" (" + std::to_wstring(elapsedTime.asMilliseconds()) + L"ms)"
#else
		+ L" (Seed: " + std::to_wstring(rng.getSeed()) + L")"
#endif
	);

	tilemap->load(map);
}

void Application::registerGenerators()
{
	generators.emplace_back(std::make_unique<OldForest>());
	generators.emplace_back(std::make_unique<MazyForest>());
	generators.emplace_back(std::make_unique<ForestAndLakes>());
	// generators.emplace_back(std::make_unique<ForestAndRooms>());
	generators.emplace_back(std::make_unique<ForestAndRiver>());

	generators.emplace_back(std::make_unique<BigCave>());
	generators.emplace_back(std::make_unique<ConnectedCaves>());
	// generators.emplace_back(std::make_unique<CavesAndBridges>());
	generators.emplace_back(std::make_unique<CavesAndLava>());
	generators.emplace_back(std::make_unique<CavesAndMazes>());
	generators.emplace_back(std::make_unique<CavesAndRooms>());

	generators.emplace_back(std::make_unique<ClassicDungeon>());
	generators.emplace_back(std::make_unique<BSPDugeon>());
	generators.emplace_back(std::make_unique<RoomsAndMazes>());

	generators.emplace_back(std::make_unique<SpaceshipA>());
	generators.emplace_back(std::make_unique<SpaceshipB>());
	// generators.emplace_back(std::make_unique<LunarBase>());
}
