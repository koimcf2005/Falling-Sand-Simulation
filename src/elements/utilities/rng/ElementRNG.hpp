// src/elements/utilities/rng/ElementRNG.hpp
#ifndef ELEMENT_RNG_HPP
#define ELEMENT_RNG_HPP

#include <random>

/**
 * @brief RNG utilities class for Elements
 * 
 * Provides reusable, static helper functions for element behavior that involves randomness,
 * such as probability checks or choosing a random horizontal direction.
 */
class ElementRNG {
public:
	/**
	 * @brief Returns true with the given chance percentage.
	 * 
	 * @param percentage A value between 0.0 and 1.0 representing the probability of returning true.
	 * @return true if the random chance succeeds; false otherwise.
	 */
	static bool getRandomChance(float percentage);

	/**
	 * @brief Returns a random horizontal direction (-1 or +1).
	 * 
	 * Useful for selecting a random left/right movement.
	 * @return -1 or +1 with equal probability.
	 */
	static int getRandomDirection();

	/**
	 * @brief Returns a random integer in the range [min, max] (inclusive).
	 * 
	 * @param min The minimum value (inclusive).
	 * @param max The maximum value (inclusive).
	 * @return A random integer between min and max.
	 */
	static int getRandomInt(int min, int max);

	/**
	 * @brief Returns a random float in the range [min, max] (inclusive).
	 * 
	 * @param min The minimum value (inclusive).
	 * @param max The maximum value (inclusive).
	 * @return A random float between min and max.
	 */
	static float getRandomFloat(float min, float max);

private:
	/// Random number generator instance (shared by all calls).
	static std::mt19937 s_RNG;
};

#endif // ELEMENT_RNG_HPP
