#include "Rng.hpp"

#include <iostream>

Rng::Rng(unsigned int seed)
	: seed(seed)
	, mt(seed)
{
	std::cout << "RNG Seed: 0x" << std::hex << seed << std::dec << '\n';
}

void Rng::reset()
{
	setSeed(std::random_device()());
}

void Rng::setSeed(unsigned int seed)
{
	this->seed = seed;
	mt.seed(seed);

	std::cout << "RNG Seed: 0x" << std::hex << seed << std::dec << '\n';
}

unsigned int Rng::getSeed() const
{
	return seed;
}

int Rng::getInt(int exclusiveMax)
{
	return std::uniform_int_distribution<>(0, exclusiveMax - 1)(mt);
}

int Rng::getInt(int min, int inclusiveMax)
{
	return min + std::uniform_int_distribution<>(0, inclusiveMax - min)(mt);
}

bool Rng::getBool(double probability)
{
	return std::bernoulli_distribution(probability)(mt);
}

float Rng::getFloat(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(mt);
}

int Rng::rollDice(int n, int s)
{
	int result = 0;

	for (int i = 0; i < n; ++i)
		result += getInt(1, s);

	return result;
}
