#ifndef SMOKE_HPP
#define SMOKE_HPP

#include "Gas.hpp"

class Smoke : public Gas {
public:
	Smoke(int x, int y) : Gas(SMOKE, x, y) {}
};

#endif // SAND_HPP
