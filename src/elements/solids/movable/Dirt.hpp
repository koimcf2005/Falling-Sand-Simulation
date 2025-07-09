#ifndef DIRT_HPP
#define DIRT_HPP

#include "src/elements/solids/movable/MovableSolid.hpp"

class Dirt : public MovableSolid {
public:
	Dirt(int x, int y) : MovableSolid(DIRT, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.9f;
		impactAbsorption = 0.75f;
		inertialResistance = 0.2f;
		density = 1.0f;
	}
};

#endif // DIRT_HPP
