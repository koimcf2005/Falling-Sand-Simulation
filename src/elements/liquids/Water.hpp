#ifndef WATER_HPP
#define WATER_HPP

#include "src/elements/liquids/Liquid.hpp"

class Water : public Liquid {
public:
	Water(int x, int y) : Liquid(WATER, x, y) {
		density = 0.5f;
	}
};

#endif // SAND_HPP
