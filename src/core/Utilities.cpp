#include "src/core/Utilities.hpp"
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