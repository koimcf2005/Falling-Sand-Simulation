// Utilities.hpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// Declares utility functions for random number generation and other helpers
// used throughout the simulation.

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