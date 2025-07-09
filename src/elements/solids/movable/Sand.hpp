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
	void reactToHeat(IMatrix& matrix, int x, int y) override;
};

void Sand::reactToHeat(IMatrix& matrix, int x, int y) {
    // Sand turns reddish when very hot (visual effect)
    if (temperature > 500.0f) {
        color = {255, 100, 50, 255}; // Hot sand color
    } else {
        color = backup_color; // Return to normal color when cooled
    }
}

#endif // SAND_HPP
