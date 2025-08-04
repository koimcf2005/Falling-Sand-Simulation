// Utilities.cpp
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
// Implements utility functions for random number generation and other helpers
// used throughout the simulation.

#include "falling-sand-sim/utils/Utilities.hpp"

#include <algorithm>

std::mt19937 RNG::s_rng{std::random_device{}()};

bool RNG::getRandomChance(const float percent) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(s_rng) <= std::clamp(percent, 0.0f, 1.0f);
}

int RNG::getRandomDirection() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(s_rng) == 0 ? -1 : 1;
}

float RNG::getRandomFloat(const float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_rng);
}

int RNG::getRandomInt(const int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(s_rng);
}