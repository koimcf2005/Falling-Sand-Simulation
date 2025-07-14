#ifndef WOOD_HPP
#define WOOD_HPP

#include "src/elements/Element.hpp"
#include "src/elements/solids/immoveable/ImmoveableSolid.hpp"

class Wood : public ImmoveableSolid {
public:
	Wood(int x, int y) : ImmoveableSolid(WOOD, x, y) {
		temperatureThreshold = 600.0f;
	}

	void reactToHeat(IMatrix& matrix, int x, int y) override;
};

void Wood::reactToHeat(IMatrix& matrix, int x, int y) {
	// matrix.placeElement(x, y, FIRE);
}

#endif // WOOD_HPP
