#include "Utility.hpp"

#include <random>
#include <cmath> // sqrt
#include <cstdlib> // abs
#include <utility> // swap

namespace
{
	std::random_device rd;
	std::mt19937 mt(rd());
}

int odd(int number)
{
	return number / 2 * 2 + 1;
}

int even(int number)
{
	return number / 2 * 2;
}

int randomInt(int exclusiveMax)
{
	return std::uniform_int_distribution<>(0, exclusiveMax - 1)(mt);
}

int randomInt(int min, int inclusiveMax)
{
	return min + std::uniform_int_distribution<>(0, inclusiveMax - min)(mt);
}

int length(const sf::Vector2i& vector)
{
	return static_cast<int>(std::sqrt(vector.x * vector.x + vector.y * vector.y));
}

int lengthSquared(const sf::Vector2i& vector)
{
	return vector.x * vector.x + vector.y * vector.y;
}

std::vector<sf::Vector2i> getLine(const sf::Vector2i& from, const sf::Vector2i& to, bool orthogonalSteps)
{
	sf::Vector2i delta = to - from;
	sf::Vector2i primaryIncrement(sign(delta.x), 0);
	sf::Vector2i secondaryIncrement(0, sign(delta.y));
	int primary = std::abs(delta.x);
	int secondary = std::abs(delta.y);

	if (secondary > primary)
	{
		std::swap(primary, secondary);
		std::swap(primaryIncrement, secondaryIncrement);
	}

	std::vector<sf::Vector2i> line;
	sf::Vector2i current = from;
	int error = 0;

	while (true)
	{
		line.emplace_back(current);

		if (current == to)
			break;

		current += primaryIncrement;
		error += secondary;

		if (error * 2 >= primary)
		{
			if (orthogonalSteps)
				line.emplace_back(current);

			current += secondaryIncrement;
			error -= primary;
		}
	}

	return line;
}
