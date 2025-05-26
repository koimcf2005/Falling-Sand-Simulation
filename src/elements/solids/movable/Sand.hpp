#ifndef SAND_HPP
#define SAND_HPP

#include "MovableSolid.hpp"

class Sand : public MovableSolid {
public:
	Sand(int x, int y) : MovableSolid(SAND, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.9f;
		absorption = 0.5f;
		inertialResistance = 0.05f;
	}
};

#endif // SAND_HPP
