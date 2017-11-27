#include "Forest.hpp"
#include "../Map/Map.hpp"
#include "../Utility/Rng.hpp"
#include "../Utility/Direction.hpp"
#include "../Utility/Utility.hpp"

#include <utility> // swap

Forest::Forest()
{
	floor = Tile::Grass;
	wall = Tile::Tree;
	corridor = Tile::Grass;
}

void Forest::generateForest(int numMeadows, int voronoiIterations, int meadowRadius)
{
	// Forest from Hauberk
	// https://github.com/munificent/hauberk/blob/master/lib/src/content/forest.dart

	map->fill(wall);

	std::vector<Point> meadows;

	for (int i = 0; i < numMeadows; ++i)
	{
		int x = rng->getInt(1, width - 1);
		int y = rng->getInt(1, height - 1);

		meadows.emplace_back(x, y);
	}

	for (int i = 0; i < voronoiIterations; ++i)
		relaxation(meadows);

	for (const auto& meadow : meadows)
		carveCircle(meadow, meadowRadius);

	connectPoints(meadows);

	erode(10000);
}

void Forest::generateLakes(int wallProb, int removeProb)
{
	std::swap(floor, water);

	fill(wallProb);

	for (int i = 0; i < 4; ++i)
		generation(5, 2);

	for (int i = 0; i < 3; ++i)
		generation(5);

	removeRegions(removeProb, 10);

	std::swap(floor, water);
}

void Forest::generateRivers(int numRivers, int perturbation)
{
	std::swap(floor, water);

	for (int i = 0; i < numRivers; ++i)
	{
		Point from(rng->getInt(width), rng->getInt(height));
		Point to(rng->getInt(width), rng->getInt(height));

		extendLine(from, to);
		carveWindingRoad(from, to, perturbation);
	}

	std::swap(floor, water);
}

void Forest::smoothMap(int floorNoise)
{
	for (int y = 1; y < height - 1; ++y)
		for (int x = 1; x < width - 1; ++x)
		{
			if (map->getTile(x, y) == wall && rng->getInt(100) < floorNoise)
				map->setTile(x, y, floor);
		}

	for (int i = 0; i < 4; ++i)
		generation(5, 2);

	for (int i = 0; i < 3; ++i)
		generation(5);
}

void OldForest::onGenerate()
{
	setName(L"Old forest");

	generateForest(10, 5, 4);
	smoothMap(35);

	connectRegions(10);
}

void MazyForest::onGenerate()
{
	setName(L"Mazy forest");

	// Generate mazes
	generateLakes(55, 0);

	// Carve shorelines
	for (int y = 1; y < height - 1; ++y)
		for (int x = 1; x < width - 1; ++x)
		{
			if (map->getTile(x, y) != wall)
				continue;

			for (const auto& dir : Direction::All)
			{
				if (map->getTile(x + dir.x, y + dir.y) == water)
				{
					map->setTile(x, y, floor);
					break;
				}
			}
		}
	
	erode(10000);
	erodeTiles(wall, floor, 5);

	connectRegions(); // TODO: narrow paths - override carvePath?

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) == water)
				map->setTile(x, y, wall);
		}
}

void ForestAndLakes::onGenerate()
{
	setName(L"Forest and lakes");

	generateForest(9, 5, 3);
	smoothMap(35);

	std::vector<Tile> forest = map->copy();

	generateLakes(55, 75);

	// Merge forest and lakes
	for (int y = 1; y < height - 1; ++y)
		for (int x = 1; x < width - 1; ++x)
		{
			if (map->getTile(x, y) == wall && forest[x + y * width] == floor)
				map->setTile(x, y, floor);
		}

	// UNDONE: Carve shorelines
	for (int y = 1; y < height - 1; ++y)
		for (int x = 1; x < width - 1; ++x)
		{
			if (map->getTile(x, y) != wall)
				continue;

			int waterWeight = 0;

			for (int dy = -2; dy <= 2; ++dy)
				for (int dx = -2; dx <= 2; ++dx)
				{
					int ax = std::abs(dx);
					int ay = std::abs(dy);

					if (ax == 2 && ay == 2)
						continue;

					if (!map->isInBounds(x + dx, y + dy))
						continue;

					if (map->getTile(x + dx, y + dy) == water)
						waterWeight += 4 / (ax + ay); // ax + ay = distance
				}

			if (waterWeight >= 4)
				map->setTile(x, y, floor);
			
			else if (waterWeight > 0 && rng->getInt(9) == 0)
				map->setTile(x, y, floor);
		}

	erodeTiles(wall, floor, 5);
	
	connectRegions(10);
	constructBridges(5);
}

/*
// UNDONE:
void ForestAndRooms::onGenerate()
{
	setName(L"Forest and rooms");

	generateForest(10, 5, 4);

	// Generate irregular-shaped rooms
	for (int i = 0; i < 1000; ++i)
	{
		Room room;
		room.width = rng->rollDice(4, 4);
		room.height = rng->rollDice(4, 4);
		room.left = rng->getInt(1, width - room.width - 1);
		room.top = rng->getInt(1, height - room.height - 1);

		if (canCarve(room))
		{
			fill(room, 45);
			
			for (int i = 0; i < 4; ++i)
				generation(room, 5);
		}
	}

	connectRegions(10, PathType::Corridor);
}
*/

void ForestAndRiver::onGenerate()
{
	setName(L"Forest and a river");

	generateForest(9, 5, 3);
	smoothMap(35);

	std::vector<Tile> forest = map->copy();

	// generateLakes(55, 75);
	generateRivers(1, 1000);

	// TODO: Remove duplicated code from ForestAndLakes
	for (int y = 1; y < height - 1; ++y)
		for (int x = 1; x < width - 1; ++x)
		{
			if (map->getTile(x, y) != wall)
				continue;

			if (forest[x + y * width] == floor)
			{
				map->setTile(x, y, floor);
				continue;
			}

			int waterWeight = 0;

			for (int dy = -2; dy <= 2; ++dy)
				for (int dx = -2; dx <= 2; ++dx)
				{
					int ax = std::abs(dx);
					int ay = std::abs(dy);

					if (ax == 2 && ay == 2)
						continue;

					if (!map->isInBounds(x + dx, y + dy))
						continue;

					if (map->getTile(x + dx, y + dy) == water)
						waterWeight += 4 / (ax + ay); // ax + ay = distance
				}

			if (waterWeight >= 4)
				map->setTile(x, y, floor);

			else if (waterWeight > 0 && rng->getInt(9) == 0)
				map->setTile(x, y, floor);
		}

	erodeTiles(wall, floor, 5);

	connectRegions(10);
	constructBridges(5);
}
