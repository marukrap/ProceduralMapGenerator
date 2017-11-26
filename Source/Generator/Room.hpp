#pragma once

#include <SFML/System/Vector2.hpp>

struct Room
{
	int right() const;
	int bottom() const;

	bool contains(int x, int y) const;
	bool contains(const sf::Vector2i& pos) const;

	bool intersects(const Room& room) const;

	int x, y, width, height;
};
