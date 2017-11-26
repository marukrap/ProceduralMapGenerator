#include "Map.hpp"

#include <algorithm> // fill
#include <utility> // move

Map::Map(int width, int height)
	: width(width)
	, height(height)
	, tiles(width * height, Tile::Unused)
{
}

bool Map::isInBounds(int x, int y) const
{
	return x >= 0 && x < width && y >= 0 && y < height;
}

bool Map::isInBounds(const sf::Vector2i& pos) const
{
	return isInBounds(pos.x, pos.y);
}

void Map::setTile(int x, int y, Tile tile)
{
	tiles[x + y * width] = tile;
}

void Map::setTile(const sf::Vector2i& pos, Tile tile)
{
	setTile(pos.x, pos.y, tile);
}

Tile Map::getTile(int x, int y) const
{
	return tiles[x + y * width];
}

Tile Map::getTile(const sf::Vector2i& pos) const
{
	return getTile(pos.x, pos.y);
}

void Map::fill(Tile tile)
{
	std::fill(tiles.begin(), tiles.end(), tile);
}

void Map::move(std::vector<Tile>&& tiles)
{
	this->tiles = std::move(tiles);
}

std::vector<Tile> Map::copy()
{
	return tiles;
}
