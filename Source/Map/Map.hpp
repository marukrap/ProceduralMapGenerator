#pragma once

#include "Tile.hpp"

#include <SFML/System/Vector2.hpp>

#include <vector>

class Map
{
public:
	Map(int width, int height);

	bool isInBounds(int x, int y) const;
	bool isInBounds(const sf::Vector2i& pos) const;

	void setTile(int x, int y, Tile tile);
	void setTile(const sf::Vector2i& pos, Tile tile);

	Tile getTile(int x, int y) const;
	Tile getTile(const sf::Vector2i& pos) const;

	void fill(Tile tile);

	// TODO: Better names?
	void move(std::vector<Tile>&& tiles);
	std::vector<Tile> copy();

public:
	const int width, height;

private:
	std::vector<Tile> tiles;
};
