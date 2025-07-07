#ifndef COAL_HPP
#define COAL_HPP

#include "MovableSolid.hpp"

class Coal : public MovableSolid {
public:
	Coal(int x, int y) : MovableSolid(COAL, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.9f;
		absorption = 0.1f;
		inertialResistance = 0.4f;
		density = 1.0f;
	}
};

#endif // COAL_HPP
