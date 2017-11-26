#include "Dungeon.hpp"
#include "../Map/Map.hpp"
#include "../Utility/Rng.hpp"
#include "../Utility/Utility.hpp"
#include "../Utility/Direction.hpp"

#include <queue>

void Dungeon::placeDoors(int doorProb)
{
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) != corridor)
				continue;

			for (const auto& dir : Direction::Cardinal)
			{
				if (map->getTile(x + dir.x, y + dir.y) == floor)
				{
					if (rng->getInt(100) < doorProb)
					{
						if (rng->getInt(3) > 0)
							map->setTile(x, y, Tile::ClosedDoor);
						else
							map->setTile(x, y, Tile::OpenDoor);
					}

					break;
				}
			}
		}
}

void Dungeon::removeCorridors()
{
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) == corridor)
				map->setTile(x, y, floor);
		}
}

void ClassicDungeon::onGenerate()
{
	// NOTE: Original Rogue-like dungeon

	setName(L"Classic dungeon");

	map->fill(wall);

	std::vector<Point> points;

	for (int i = 0; i < 15; ++i)
	{
		int x = rng->getInt(1, width - 2);
		int y = rng->getInt(1, height - 2);

		points.emplace_back(x, y);
	}

	for (int i = 0; i < 5; ++i)
		relaxation(points);

	for (auto& point : points)
	{
		point.x = odd(point.x);
		point.y = odd(point.y);
	}

	for (auto it = points.begin(); it != points.end(); )
	{
		auto point = *it;

		Room room;
		room.width = odd(rng->rollDice(4, 3));
		room.height = odd(rng->rollDice(3, 3));
		room.left = odd(std::min(std::max(1, point.x - room.width / 2), width - room.width - 2));
		room.top = odd(std::min(std::max(1, point.y - room.height / 2), height - room.height - 2));
		
		point.x = odd(room.left + room.width / 2);
		point.y = odd(room.top + room.height / 2);

		if (canCarve(room))
		{
			carveRoom(room);
			++it;
		}

		else
			it = points.erase(it);
	}

	connectPoints(points, PathType::Corridor);
	removeWalls();

	placeDoors(100);
	removeCorridors();

	return;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) != corridor)
				continue;

			for (const auto& dir : Direction::Cardinal)
			{
				if (map->getTile(x + dir.x, y + dir.y) == floor)
				{
					if (rng->getInt(9) > 0)
						map->setTile(x, y, Tile::ClosedDoor);
					else
						map->setTile(x, y, Tile::OpenDoor);

					break;
				}
			}
		}

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			if (map->getTile(x, y) == corridor)
				map->setTile(x, y, floor);
		}
}

// UNDONE: Incomplete
void BSPDugeon::onGenerate()
{
	setName(L"BSP dungeon");
	
	map->fill(wall);

	std::queue<Room> active;
	std::vector<Room> inactive;

	active.emplace(1, 1, map->width - 2, map->height - 2);

	const int minWidth = 3;
	const int minHeight = 3;

	while (!active.empty())
	{
		Room room = active.front();
		active.pop();

		/*
		if (room.width < minWidth * 3 && room.height < minHeight * 3 && rng->getBool())
		{
			inactive.emplace_back(room);
			continue;
		}
		*/

		if (room.width > room.height || (room.width == room.height && rng->getBool()))
		{
			// Split horizontally
			if (room.width > minWidth * 2)
			{
				int midPoint = odd(rng->getInt(minWidth, room.width - minWidth - 1));
				
				active.emplace(room.left, room.top, midPoint, room.height);
				active.emplace(room.left + midPoint + 1, room.top, room.width - (midPoint + 1), room.height);
			}

			else
				inactive.emplace_back(room);
		}

		else
		{
			// Split vertically
			if (room.height > minHeight * 2)
			{
				int midPoint = odd(rng->getInt(minHeight, room.height - minHeight - 1));

				active.emplace(room.left, room.top, room.width, midPoint);
				active.emplace(room.left, room.top + midPoint + 1, room.width, room.height - (midPoint + 1));
			}

			else
				inactive.emplace_back(room);
		}
	}

	for (auto it = inactive.begin(); it != inactive.end(); )
	{
		if (rng->getBool())
			it = inactive.erase(it);
		else
			++it;
	}

	for (std::size_t i = 0; i < inactive.size(); ++i)
		carveRoom(inactive[i]);

	for (std::size_t i = 0; i < inactive.size(); ++i)
	{
		Room& r1 = inactive[i];

		if (rng->getBool(0.75))
		{
			std::vector<Room> neighbors;

			for (std::size_t j = i + 1; j < inactive.size(); ++j)
			{
				Room& r2 = inactive[j];

				Room r1h(r1.left - 1, r1.top, r1.width + 2, r1.height);
				Room r1v(r1.left, r1.top - 1, r1.width, r1.height + 2);

				Room r2h(r2.left - 1, r2.top, r2.width + 2, r2.height);
				Room r2v(r2.left, r2.top - 1, r2.width, r2.height + 2);

				Room intersection;

				if (r1h.intersects(r2h, intersection))
					neighbors.emplace_back(intersection);

				if (r1v.intersects(r2v, intersection))
					neighbors.emplace_back(intersection);
			}

			if (!neighbors.empty())
			{
				auto selected = rng->getOne(neighbors);

				if (selected.width > 1 || selected.height > 1)
					carveRoom(selected);
			}
		}
	}

	connectRegions(0, PathType::Corridor, false);
	removeWalls();

	placeDoors(25);
	removeCorridors();
}

void RoomsAndMazes::onGenerate()
{
	// Hauberk-style dungeon generation

	setName(L"Rooms and mazes");

	map->fill(wall);

	for (int i = 0; i < 100; ++i)
	{
		Room room;
		room.width = odd(rng->rollDice(3, 3));
		room.height = odd(rng->rollDice(3, 3));
		room.left = odd(rng->getInt(width - room.width - 1));
		room.top = odd(rng->getInt(height - room.height - 1));

		if (canCarve(room))
			carveRoom(room);
	}

	std::vector<Point> maze;

	for (int y = 1; y < height - 1; y += 2)
		for (int x = 1; x < width - 1; x += 2)
			growMaze(maze, x, y, 35);

	connectRegions(0, PathType::Corridor, false);
	removeDeadEnds(maze);
	removeWalls();

	placeDoors(50);
	removeCorridors();
}
