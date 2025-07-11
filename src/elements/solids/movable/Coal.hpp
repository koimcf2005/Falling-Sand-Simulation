#ifndef COAL_HPP
#define COAL_HPP

#include "src/elements/solids/movable/MovableSolid.hpp"

class Coal : public MovableSolid {
public:
	Coal(int x, int y) : MovableSolid(COAL, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.9f;
		impactAbsorption = 0.7f;
		inertialResistance = 0.4f;
		density = 1.0f;
	}
};

#endif // COAL_HPP
