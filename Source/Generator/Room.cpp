#include "Room.hpp"

#include <cstdlib> // abs

int Room::right() const
{
	return x + width;
}

int Room::bottom() const
{
	return y + height;
}

bool Room::contains(int x, int y) const
{
	return x >= this->x && x < right() && y >= this->y && y < bottom();
}

bool Room::contains(const sf::Vector2i& pos) const
{
	return contains(pos.x, pos.y);
}

bool Room::intersects(const Room& room) const
{
	return (std::abs((x + width / 2.f) - (room.x - room.width / 2.f)) < (width + room.width) / 2.f)
		&& (std::abs((y + height / 2.f) - (room.y - room.height / 2.f)) < (height + room.height) / 2.f);
}
