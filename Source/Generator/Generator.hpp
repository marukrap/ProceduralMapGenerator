#pragma once

#include "../Map/Tile.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <string>
#include <vector>

class Map;
class Rng;

class Generator
{
public:
	using Room = sf::IntRect;
	using Point = sf::Vector2i;

	enum class PathType
	{
		Straight,
		Corridor,
		WindingRoad,
	};

public:
	virtual ~Generator() = default;

	const std::wstring& getName() const;

	void generate(Map& map, Rng& rng);

protected:
	void setName(const std::wstring& name);

	// Cellular automata
	// http://www.jimrandomh.org/misc/caves.html
	void generation(int r1cutoff);
	void generation(int r1cutoff, int r2cutoff);
	void generation(const Room& room, int r1cutoff); // REMOVE:
	void generation(const Room& room, int r1cutoff, int r2cutoff); // REMOVE:

	// Remove or connect unconnected regions
	void removeRegions(int removeProb = 100, int minSize = 0);
	void connectRegions(int minSize = 0, PathType type = PathType::Straight, bool allowDiagonalSteps = true);
	void constructBridges(int minSize = 0);

	// Lloyd's algorithm (Voronoi iteration)
	void relaxation(std::vector<Point>& points);
	void connectPoints(std::vector<Point>& points, PathType type = PathType::Straight);

	// Growing tree
	void growMaze(std::vector<Point>& maze, int x, int y, int windingProb);
	void removeDeadEnds(std::vector<Point>& maze);

	//
	void fill(int wallProb);
	void fill(const Room& room, int wallProb); // REMOVE:

	bool canCarve(const Room& room) const;
	void carveRoom(const Room& room);

	void carvePath(const Point& from, const Point& to);
	void carveCorridor(const Point& from, const Point& to);
	void carveCircle(const Point& center, int radius);

	// Used for river generation
	void carveWindingRoad(const Point& from, const Point& to, int perturbation = 10);
	void extendLine(Point& from, Point& to);

	void erode(int iterations);
	void erodeTiles(Tile from, Tile to, int r1cutoff);

	void removeWalls(); // remove unused walls

private:
	virtual void onGenerate() = 0;

	int countTiles(Tile tile, int x, int y) const; // count adjacent tiles

// private:
protected:
	Map* map = nullptr;
	Rng* rng = nullptr;

	int width = 0;
	int height = 0;

	Tile unused = Tile::Unused;
	Tile floor = Tile::Floor;
	Tile corridor = Tile::Corridor;
	Tile wall = Tile::Wall;
	Tile water = Tile::Water;
	Tile bridge = Tile::Bridge;

private:
	std::wstring name;
};
