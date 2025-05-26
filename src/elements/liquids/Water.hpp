#ifndef WATER_HPP
#define WATER_HPP

#include "Liquid.hpp"

class Water : public Liquid {
public:
	Water(int x, int y) : Liquid(WATER, x, y) {}
};

#endif // SAND_HPP
