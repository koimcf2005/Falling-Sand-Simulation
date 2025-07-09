#ifndef OIL_HPP
#define OIL_HPP

#include "src/elements/liquids/Liquid.hpp"

class Oil : public Liquid {
public:
	Oil(int x, int y) : Liquid(OIL, x, y) {
		density = 0.4f;
	}
	void reactToHeat(IMatrix& matrix, int x, int y) override;
};

void Oil::reactToHeat(IMatrix& matrix, int x, int y) {
    // Oil ignites easily
    if (norm_dist(rng) < 0.8f) { // 80% chance to ignite
        matrix.placeElement(x, y, FIRE);
    }
}

#endif // SAND_HPP
