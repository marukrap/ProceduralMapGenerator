#pragma once

#include "Generator.hpp"

class Forest : public Generator
{
public:
	Forest();

protected:
	void generateForest(int numMeadows, int voronoiIterations, int meadowRadius);
	void generateLakes(int wallProb, int removeProb);
	void generateRivers(int numRivers, int perturbation);

	void smoothMap(int floorNoise);
};

class OldForest : public Forest
{
protected:
	void onGenerate() override;
};

class MazyForest : public Forest
{
private:
	void onGenerate() override;
};

class ForestAndLakes : public Forest
{
private:
	void onGenerate() override;
};

/*
class ForestAndRooms : public Forest
{
private:
	void onGenerate() override;
};
*/

class ForestAndRiver : public Forest
{
private:
	void onGenerate() override;
};
