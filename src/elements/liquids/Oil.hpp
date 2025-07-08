#ifndef OIL_HPP
#define OIL_HPP

#include "src/elements/liquids/Liquid.hpp"

class Oil : public Liquid {
public:
	Oil(int x, int y) : Liquid(OIL, x, y) {
		density = 0.4f;
	}
};

#endif // SAND_HPP
