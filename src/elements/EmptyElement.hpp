#ifndef EMPTY_HPP
#define EMPTY_HPP

#include "src/elements/Element.hpp"

class Empty : public Element {
	public:
		Empty(int x, int y) : Element(EMPTY, x, y) {
			density = 0.0f;
			velocity_y = 0.0f;
			accumulated_y = 0.0f;
			friction = 0.0f;
			impactAbsorption = 0.0f;
			inertialResistance = 0.0f;

			// Movement Flags
			wasMoving = false;
			isMoving = false;
			movedThisFrame = false;
			step = false;
		}

		void update(IMatrix&, int, int) override {
			return;
		}
};

#endif // EMPTY_HPP
