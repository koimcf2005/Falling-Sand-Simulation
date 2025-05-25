#ifndef SAND_HPP
#define SAND_HPP

#include "MovableSolid.hpp"

class Sand : public MovableSolid {
public:
	Sand(int x, int y) : MovableSolid(SAND, x, y) {}
};

#endif // SAND_HPP
