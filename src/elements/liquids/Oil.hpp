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
	// Only combust if none of the neighbors are fire,
	// because if the neighbors are fire,
	// then the fire logic will consume the wood instead
	if (Element::getNeighborElementsOfType<Element>(matrix, FIRE, x, y).size() == 0) {
		matrix.placeElement(x, y, FIRE);
	}
}

#endif // SAND_HPP
