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
	if (dissolvedElement == EMPTY || destroyDissolvedElement) {
		// Water turns to steam when heated enough
		matrix.placeElement(x, y, STEAM);
	}
	else {
		matrix.placeElement(x, y, dissolvedElement);
	}

}

#endif // SAND_HPP
