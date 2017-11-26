#pragma once

#include <SFML/System/Vector2.hpp>

#include <array>

class Direction : public sf::Vector2i
{
public:
	using sf::Vector2i::Vector2;

	Direction left45() const;
	Direction right45() const;

	Direction left90() const;
	Direction right90() const;

public:
	static const Direction None;
	static const Direction N;
	static const Direction NE;
	static const Direction E;
	static const Direction SE;
	static const Direction S;
	static const Direction SW;
	static const Direction W;
	static const Direction NW;

	static const std::array<Direction, 8> All;
	static const std::array<Direction, 4> Cardinal;
	static const std::array<Direction, 4> Diagonal;
};
