#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <algorithm>
#include <random>

class RNG {
public:
	static bool getRandomChance(const float percent);
	static int getRandomDirection();
	static float getRandomFloat(const float min, float max);
	static int getRandomInt(const int min, int max);
	static std::mt19937 s_rng;
};

#endif // UTILITIES_HPP