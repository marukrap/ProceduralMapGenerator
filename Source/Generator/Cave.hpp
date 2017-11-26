#pragma once

#include "Generator.hpp"

class Cave : public Generator
{
public:
	Cave();

protected:
	void removeWallChunks(int minSize = 10);
};

class BigCave : public Cave
{
private:
	void onGenerate() override;
};

class ConnectedCaves : public Cave
{
private:
	void onGenerate() override;
};

/*
class CavesAndBridges : public Cave
{
private:
	void onGenerate() override;
};
*/

class CavesAndMazes : public Cave
{
private:
	void onGenerate() override;
};

class CavesAndLava : public Cave
{
private:
	void onGenerate() override;
};

class CavesAndRooms : public Cave
{
private:
	void onGenerate() override;
};
