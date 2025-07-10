#ifndef ASH_HPP
#define ASH_HPP

#include "src/elements/solids/movable/MovableSolid.hpp"

class Ash : public MovableSolid {
public:
	Ash(int x, int y) : MovableSolid(ASH, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.6f;
		impactAbsorption = 0.8f;
		inertialResistance = 0.025f;
		density = 0.4f;
	}
};

#endif // ASH_HPP
