#ifndef SALT_HPP
#define SALT_HPP

#include "src/elements/solids/movable/MovableSolid.hpp"

class Salt : public MovableSolid {
public:
	Salt(int x, int y) : MovableSolid(SALT, x, y) {
		velocity_y = 1.0f;
		velocity_x = 0.0f;
		friction = 0.9f;
		impactAbsorption = 0.5f;
		inertialResistance = 0.05f;
		density = 0.4f;
		dissolveChances = {
			{ WATER, 0.01f }
		};
		dissolveThreshold = 3;
	}
};

#endif // SAND_HPP
