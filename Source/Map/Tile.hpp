#pragma once

enum class Tile
{
	Unused,

	/* Dungeon */
	Floor,
	Corridor,
	Wall,
	ClosedDoor,
	OpenDoor,
	UpStairs,
	DownStairs,

	/* Forest */
	Grass,
	Tree,
	ClosedGate,
	OpenGate,
	Water,
	Bridge,

	/* Cave */
	Dirt,
	CaveWall,
	Lava,
	// Pit,

	/* Spaceship */
	Void,
	VoidWall,
};
