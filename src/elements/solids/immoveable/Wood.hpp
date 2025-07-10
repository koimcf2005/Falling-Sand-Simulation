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
	// Only combust if none of the neighbors are fire,
	// because if the neighbors are fire,
	// then the fire logic will consume the wood instead
	if (Element::getNeighborElementsOfType<Element>(matrix, FIRE, x, y).size() == 0) {
		matrix.placeElement(x, y, FIRE);
	}
}

#endif // WOOD_HPP
