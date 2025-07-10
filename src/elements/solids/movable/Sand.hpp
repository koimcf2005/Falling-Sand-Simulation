#ifndef SAND_HPP
#define SAND_HPP

#include "src/elements/solids/movable/MovableSolid.hpp"

class Sand : public MovableSolid {
public:
	Sand(int x, int y) : MovableSolid(SAND, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.6f;
		impactAbsorption = 0.8f;
		inertialResistance = 0.025f;
		density = 0.49f;
	}
};

#endif // SAND_HPP
