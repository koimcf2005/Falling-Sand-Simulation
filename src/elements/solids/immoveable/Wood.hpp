#ifndef WOOD_HPP
#define WOOD_HPP

#include "src/elements/Element.hpp"
#include "src/elements/solids/immoveable/ImmoveableSolid.hpp"

class Wood : public ImmoveableSolid {
public:
	Wood(int x, int y) : ImmoveableSolid(WOOD, x, y) {}

	void reactToHeat(IMatrix& matrix, int x, int y) override;
};

void Wood::reactToHeat(IMatrix& matrix, int x, int y) {
	// Wood catches fire when heated
	if (norm_dist(rng) < 0.01f) { // 30% chance to ignite
		matrix.placeElement(x, y, FIRE);
	}
}

#endif // WOOD_HPP
