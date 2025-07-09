#ifndef WATER_HPP
#define WATER_HPP

#include "src/elements/liquids/Liquid.hpp"

class Water : public Liquid {
public:
	Water(int x, int y) : Liquid(WATER, x, y) {
		density = 0.5f;
	}
	void reactToHeat(IMatrix& matrix, int x, int y) override;
};

void Water::reactToHeat(IMatrix& matrix, int x, int y) {
    // Water turns to steam when heated enough
    if (temperature > 100.0f && norm_dist(rng) < 0.1f) {
        matrix.placeElement(x, y, STEAM);
    }
}

#endif // SAND_HPP
