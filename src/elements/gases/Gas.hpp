// -------------------------------------------
// Gas.hpp
// -------------------------------------------
// Base class for all gas elements (e.g., steam, smoke).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef GAS_HPP
#define GAS_HPP

#include "Element.hpp"

class Gas : public Element {
	public:
		Gas(ElementType type, int x, int y) : Element(type, x, y) {}
		// Update function to be implemented by derived classes
		void update(IMatrix& matrix, int x, int y) override;
		
	protected:
		// Gravitational constant affecting the solid
		const float UPWARD_MOMENTUM = 0.1f;
		// Velocity (cells moved per update)
		float velocity_x = 0.0f;
		float velocity_y = -1.0f;
		// Accumulated sub-cell movement for smooth simulation
		float accumulated_x = 0.0f;
		float accumulated_y = 0.0f;
		// Counter for how many updates the solid has been stopped
		int stoppedMovingCount = 0;
		// Threshold for determining when the solid is considered stopped
		int stoppedMovingThreshold = 5;
		// Flag indicating whether the solid is free falling
		bool isMoving = true;

		float chanceOfHorizontal = 0.75f;

		float chanceOfDeath = 0.01f;
};

#endif