#pragma once

#include "Generator.hpp"

class Spaceship : public Generator
{
public:
	Spaceship();
	
protected:
	void generateSpaceship(bool horizontal);

private:
	void designHull(bool horizontal);
	
	void addNoise(Tile from, Tile to, int prob);
	void smoothMap(int iterations, int threshold);
	
	void addRooms(int tries);
};

class SpaceshipA : public Spaceship
{
private:
	void onGenerate() override;
};

class SpaceshipB : public Spaceship
{
private:
	void onGenerate() override;
};

/*
class LunarBase : public Generator
{
private:
	void onGenerate() override;
};
*/
