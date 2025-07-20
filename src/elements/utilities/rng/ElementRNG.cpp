// src/elements/utilities/rng/ElementRNG.cpp
#include "src/elements/utilities/rng/ElementRNG.hpp"
#include <algorithm>

// Static member definitions
std::mt19937 ElementRNG::s_RNG{std::random_device{}()};

bool ElementRNG::getRandomChance(float percentage) {
	// Clamp percentage to [0.0, 1.0] to ensure safe probability input
	percentage = std::clamp(percentage, 0.0f, 1.0f);
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return dist(s_RNG) <= percentage;
}

int ElementRNG::getRandomDirection() {
	// Randomly return either -1 or +1
	std::uniform_int_distribution<int> dist(0, 1);
	return dist(s_RNG) == 0 ? -1 : 1;
}

int ElementRNG::getRandomInt(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(s_RNG);
}

float ElementRNG::getRandomFloat(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(s_RNG);
}