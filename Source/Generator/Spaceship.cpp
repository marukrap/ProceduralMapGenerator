#include "Spaceship.hpp"
#include "../Map/Map.hpp"
#include "../Utility/Rng.hpp"
#include "../Utility/Direction.hpp"
#include "../Utility/Utility.hpp"

#include <cstdlib> // abs
#include <algorithm> // max

Spaceship::Spaceship()
{
	unused = Tile::Void;
	wall = Tile::VoidWall;
}

void Spaceship::generateSpaceship(bool horizontal)
{
	// Generate the hull
	designHull(horizontal);

	addNoise(wall, floor, 80);
	addNoise(floor, wall, 30);

	smoothMap(10, 8);

	std::vector<Tile> temp = map->copy();

	// Generate rooms
	map->fill(wall);

	addRooms(1000);

	int floors = 0;

	// Remove room tiles outside the hull
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (temp[x + y * width] == wall)
				map->setTile(x, y, wall);
			else if (map->getTile(x, y) == floor)
				floors += 1;
		}

	// Regenerate if the map is too small
	if (floors < 400)
		generateSpaceship(horizontal);

	// Erase the half of the map
	if (horizontal)
	{
		for (int y = 0; y < height; ++y)
			for (int x = width / 2 + 1; x < width; ++x)
				map->setTile(x, y, wall);
	}

	else
	{
		for (int y = height / 2 + 1; y < height; ++y)
			for (int x = 0; x < width; ++x)
				map->setTile(x, y, wall);
	}

	// TODO: More wider corridors
	connectRegions(10, PathType::Corridor);

	// Mirror the map horizontally or vertically
	if (horizontal)
	{
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width / 2; ++x)
				map->setTile(width - 1 - x, y, map->getTile(x, y));
	}

	else
	{
		for (int y = 0; y < height / 2; ++y)
			for (int x = 0; x < width; ++x)
				map->setTile(x, height - 1 - y, map->getTile(x, y));
	}

	connectRegions(0, PathType::Corridor);

	removeWalls();
}

void Spaceship::designHull(bool horizontal)
{
	// TODO: Make all shapes have the same area size (tiles)

	int i = rng->getInt(horizontal ? 5 : 6);

	if (i == 0)
	{
		sf::Vector2i radius(width * 3 / 8, height * 3 / 8);

		// Design ellipse (1397 tiles)
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				if (dx * dx * radius.y * radius.y + dy * dy * radius.x * radius.x < radius.x * radius.x * radius.y * radius.y)
					map->setTile(x, y, floor);
				else
					map->setTile(x, y, wall);
			}
	}

	else if (i == 1)
	{
		sf::Vector2i radius(width / 2, height / 2);
		sf::Vector2i radius2(width / 4, height / 4);

		// Design torus (1936 tiles) // NOTE: too big
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				if (dx * dx * radius2.y * radius2.y + dy * dy * radius2.x * radius2.x < radius2.x * radius2.x * radius2.y * radius2.y)
					map->setTile(x, y, wall);
				else if (dx * dx * radius.y * radius.y + dy * dy * radius.x * radius.x < radius.x * radius.x * radius.y * radius.y)
					map->setTile(x, y, floor);
				else
					map->setTile(x, y, wall);
			}
	}

	else if (i == 2)
	{
		// Design rectangle (1421 tiles)
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				if (std::abs(dx) < width / 3 && std::abs(dy) < height / 3)
					map->setTile(x, y, floor);
				else
					map->setTile(x, y, wall);
			}
	}

	else if (i == 3)
	{
		// Design rectangular ring (1572 tiles)
		/*
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				int ax = std::abs(dx);
				int ay = std::abs(dy);

				if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
					map->setTile(x, y, wall);
				else if (ax < width / 4 && ay < height / 4)
					map->setTile(x, y, wall);
				else
					map->setTile(x, y, floor);
			}
		*/

		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				int ax = std::abs(dx);
				int ay = std::abs(dy);

				if (ax < width / 5 && ay < height / 5) // 20%
					map->setTile(x, y, wall);
				else if (ax < width * 2 / 5 && ay < height * 2 / 5) // 40%
					map->setTile(x, y, floor);
				else
					map->setTile(x, y, wall);
			}
	}

	else if (i == 4)
	{
		// Design hexagon (2215 tiles) // NOTE: too big
		int j = 0;

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				int dx = x - width / 2;
				int dy = y - height / 2;

				if (y == 0 || y == height - 1)
					map->setTile(x, y, wall);
				else if (std::abs(dy) < j)
					map->setTile(x, y, floor);
				else
					map->setTile(x, y, wall);
			}

			if (x < width / 2)
				++j;
			else
				--j;
		}
	}

	else // i == 5
	{
		// Design custom spaceship (1602 tiles)
		map->fill(wall);

		// Body
		for (int y = height * 3 / 8; y < height * 5 / 8; ++y)
			for (int x = 1; x < width - 1; ++x)
				map->setTile(x, y, floor);

		// Tail
		for (int y = height * 2 / 8; y < height * 6 / 8; ++y)
			for (int x = 1; x < width / 8; ++x)
				map->setTile(x, y, floor);
		
		// Wings
		for (int y = 1; y < height - 1; ++y)
			for (int x = width * 2 / 8; x < width * 3 / 8; ++x)
				map->setTile(x, y, floor);

		int wingSize = std::max(height / 8, height - (height * 7 / 8));

		// Left wing
		for (int y = 1; y < 1 + wingSize; ++y)
			for (int x = width * 2 / 8; x < width * 6 / 8; ++x)
				map->setTile(x, y, floor);

		// Right wing
		for (int y = (height - 1) - wingSize; y < height - 1; ++y)
			for (int x = width * 2 / 8; x < width * 6 / 8; ++x)
				map->setTile(x, y, floor);

		// Flip horizontally
		if (rng->getBool())
		{
			for (int y = 0; y < height; ++y)
				for (int x = 0; x < width / 2; ++x)
				{
					Tile t1 = map->getTile(x, y);
					Tile t2 = map->getTile(width - 1 - x, y);
					map->setTile(x, y, t2);
					map->setTile(width - 1 - x, y, t1);
				}
		}
	}
}

void Spaceship::addNoise(Tile from, Tile to, int prob)
{
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) == from && rng->getInt(100) < prob)
				map->setTile(x, y, to);
		}
}

void Spaceship::smoothMap(int iterations, int threshold)
{
	for (int i = 0; i < iterations; ++i)
	{
		std::vector<Tile> tiles(width * height);

		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				int r2 = 0;

				for (int dy = -2; dy <= 2; ++dy)
					for (int dx = -2; dx <= 2; ++dx)
					{
						if (std::abs(dx) + std::abs(dy) >= 3)
							continue;

						if (map->isInBounds(x + dx, y + dy) &&
							map->getTile(x + dx, y + dy) == floor)
							r2 += 1;
					}

				if (r2 >= threshold)
					tiles[x + y * width] = floor;
				else
					tiles[x + y * width] = wall;
			}

		map->move(std::move(tiles));
	}
}

void Spaceship::addRooms(int tries)
{
	// TODO: BSP-tree?

	for (int i = 0; i < tries; ++i)
	{
		Room room;
		room.width = odd(rng->rollDice(3, 3));
		room.height = odd(rng->rollDice(3, 3));
		room.left = odd(rng->getInt(1, width - room.width - 2));
		room.top = odd(rng->getInt(1, height - room.height - 2));

		if (canCarve(room))
			carveRoom(room);
	}
}

void SpaceshipA::onGenerate()
{
	setName(L"Spaceship type-A");

	generateSpaceship(false);
}

void SpaceshipB::onGenerate()
{
	setName(L"Spaceship type-B");

	generateSpaceship(true);
}

/*
void LunarBase::onGenerate()
{
	// TODO:
}
*/
