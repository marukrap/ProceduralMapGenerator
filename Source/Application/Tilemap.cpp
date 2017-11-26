#include "Tilemap.hpp"
#include "../Map/Map.hpp"
#include "../Utility/Utility.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Tilemap::Tilemap(const sf::Texture& tileset, const sf::Vector2i& tileSize)
	: tileset(tileset)
	, tileSize(tileSize)
{
}

void Tilemap::load(const Map& map)
{
	vertices.clear();
	vertices.resize(map.width * map.height * 4);
	// vertices.shrink_to_fit();

	for (int y = 0; y < map.height; ++y)
		for (int x = 0; x < map.width; ++x)
		{
			int tileNumber = getTileNumber(map, x, y);

			if (tileNumber < 0)
				continue;

			int tu = tileNumber % (tileset.getSize().x / tileSize.x);
			int tv = tileNumber / (tileset.getSize().x / tileSize.x);

			sf::Vertex* quad = &vertices[(x + y * map.width) * 4];

			quad[0].position = sf::Vector2f((x + 0.f) * tileSize.x, (y + 0.f) * tileSize.y);
			quad[1].position = sf::Vector2f((x + 1.f) * tileSize.x, (y + 0.f) * tileSize.y);
			quad[2].position = sf::Vector2f((x + 1.f) * tileSize.x, (y + 1.f) * tileSize.y);
			quad[3].position = sf::Vector2f((x + 0.f) * tileSize.x, (y + 1.f) * tileSize.y);

			quad[0].texCoords = sf::Vector2f((tu + 0.f) * tileSize.x, (tv + 0.f) * tileSize.y);
			quad[1].texCoords = sf::Vector2f((tu + 1.f) * tileSize.x, (tv + 0.f) * tileSize.y);
			quad[2].texCoords = sf::Vector2f((tu + 1.f) * tileSize.x, (tv + 1.f) * tileSize.y);
			quad[3].texCoords = sf::Vector2f((tu + 0.f) * tileSize.x, (tv + 1.f) * tileSize.y);
		}
}

void Tilemap::append(int x, int y, int tileNumber)
{
	int tu = tileNumber % (tileset.getSize().x / tileSize.x);
	int tv = tileNumber / (tileset.getSize().x / tileSize.x);

	vertices.emplace_back(sf::Vector2f((x + 0.f) * tileSize.x, (y + 0.f) * tileSize.y), sf::Vector2f((tu + 0.f) * tileSize.x, (tv + 0.f) * tileSize.y));
	vertices.emplace_back(sf::Vector2f((x + 1.f) * tileSize.x, (y + 0.f) * tileSize.y), sf::Vector2f((tu + 1.f) * tileSize.x, (tv + 0.f) * tileSize.y));
	vertices.emplace_back(sf::Vector2f((x + 1.f) * tileSize.x, (y + 1.f) * tileSize.y), sf::Vector2f((tu + 1.f) * tileSize.x, (tv + 1.f) * tileSize.y));
	vertices.emplace_back(sf::Vector2f((x + 0.f) * tileSize.x, (y + 1.f) * tileSize.y), sf::Vector2f((tu + 0.f) * tileSize.x, (tv + 1.f) * tileSize.y));
}

int Tilemap::getTileNumber(const Map& map, int x, int y)
{
	auto isWall = [&] (int x, int y)
	{
		switch (map.getTile(x, y))
		{
		case Tile::Wall:
		case Tile::CaveWall:
		case Tile::VoidWall:
		case Tile::Tree:
			return true;
		}

		return false;
	};

	auto addShadow = [&] (int x, int y)
	{
		append(x, y, 0xaf);
	};

	int tileNumber = -1;

	switch (map.getTile(x, y))
	{
	case Tile::Unused:
		tileNumber = 0x60;
		break;

	case Tile::Floor:
		tileNumber = 0x62;
		break;

	case Tile::Corridor:
		tileNumber = 0x63;
		break;

	case Tile::Wall:
		if (y == map.height - 1 || map.getTile(x, y + 1) != Tile::Wall)
		{
			tileNumber = 0x0;
			addShadow(x, y + 1);
		}

		else
			tileNumber = 0x6;

		if (randomInt(6) == 0)
		{
			if (randomInt(6) > 0)
				tileNumber += randomInt(1, 3);

			else
			{
				// tileNumber += randomInt(4, 5);

				if (tileNumber == 0x0)
					tileNumber = 0x5;
				else if (tileNumber == 0x6)
					tileNumber = 0xa;
			}
		}
		break;

	case Tile::ClosedDoor:
		tileNumber = 0x62;

		if (!isWall(x, y - 1))
		{
			append(x, y, 0xa0);
			addShadow(x, y + 1);
		}

		else
		{
			append(x, y - 1, 0xa3);
			append(x, y, 0xa2);
		}
		break;

	case Tile::OpenDoor:
		tileNumber = 0x62;

		if (!isWall(x, y - 1))
			append(x, y, 0xa1);

		else
		{
			append(x, y - 1, 0xa4);
			append(x, y, 0xa5);
		}
		break;

	case Tile::UpStairs:
		tileNumber = 0x62;
		append(x, y, 0x0d);
		break;

	case Tile::DownStairs:
		tileNumber = 0x62;
		append(x, y, 0x0c);
		break;

	case Tile::Grass:
		if (randomInt(3) > 0)
			tileNumber = 0x58;
		else if (randomInt(3) > 0)
			tileNumber = 0x58 + randomInt(1, 2);
		else
			tileNumber = 0x58 + randomInt(3, 5);

		if (randomInt(9) == 0)
		{
			tileNumber = 0x58;

			if (randomInt(9) > 0)
				append(x, y, 0xe0 + randomInt(5));
			else
				append(x, y, 0x100 + randomInt(5));
		}
		break;

	case Tile::Tree:
		tileNumber = 0x58;

		if (randomInt(3) > 0)
			append(x, y, 0x105);
		else
			append(x, y, 0x106);
		break;

	case Tile::Water:
		if (y > 0 && map.getTile(x, y - 1) != Tile::Water)
			tileNumber = 0x82 + randomInt(2);
		else
			tileNumber = 0x84 + randomInt(2);
		break;

	case Tile::ClosedGate:
		tileNumber = 0x58;

		if (!isWall(x, y - 1))
		{
			append(x, y, 0xa6);
			addShadow(x, y + 1);
		}

		else
		{
			append(x, y - 1, 0xa9);
			append(x, y, 0xa8);
		}
		break;

	case Tile::OpenGate:
		tileNumber = 0x58;

		if (!isWall(x, y - 1))
			append(x, y, 0xa7);

		else
		{
			append(x, y - 1, 0xaa);
			append(x, y, 0xab);
		}
		break;

	case Tile::Bridge:
		tileNumber = 0xbc + randomInt(3);
		break;

	case Tile::Dirt:
		if (randomInt(3) > 0)
			tileNumber = 0x50;
		else if (randomInt(3) > 0)
			tileNumber = 0x50 + randomInt(1, 3);
		else
			tileNumber = 0x50 + randomInt(4, 5);
		break;

	case Tile::CaveWall:
		if (y == map.height - 1 || map.getTile(x, y + 1) != Tile::CaveWall)
		{
			tileNumber = 0x10;
			addShadow(x, y + 1);
		}

		else
			tileNumber = 0x16;

		if (randomInt(6) == 0)
		{
			if (randomInt(6) > 0)
				tileNumber += randomInt(1, 3);
			else
				tileNumber += randomInt(4, 5);
		}
		break;

	case Tile::Lava:
		if (y > 0 && map.getTile(x, y - 1) != Tile::Lava)
			tileNumber = 0x98 + randomInt(2);
		else
			tileNumber = 0x9a + randomInt(2);
		break;

	// case Tile::Pit:
		// break;

	case Tile::Void:
		if (randomInt(9) > 0)
			tileNumber = 0x136;
		else if (randomInt(9) > 0)
			tileNumber = 0x136 + randomInt(1, 2);
		else
			tileNumber = 0x136 + randomInt(3, 5);
		break;

	case Tile::VoidWall:
		if (y == map.height - 1 || map.getTile(x, y + 1) != Tile::VoidWall)
		{
			tileNumber = 0x130;
			addShadow(x, y + 1);
		}

		else
			tileNumber = 0x133;

		if (randomInt(6) == 0)
			tileNumber += randomInt(1, 2);
		break;
	}

	return tileNumber;
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = &tileset;
	target.draw(&vertices[0], vertices.size(), sf::Quads, states);
}
