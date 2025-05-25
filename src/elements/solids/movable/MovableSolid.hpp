// -------------------------------------------
// MovableSolid.hpp
// -------------------------------------------
// Base class for all movable solid elements (e.g., sand, powder).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef MOVABLE_SOLID_HPP
#define MOVABLE_SOLID_HPP

#include "Solid.hpp"

class MovableSolid : public Solid {
	public:
		MovableSolid(ElementType type, int x, int y) : Solid(type, x, y) {}
		// Update function to be implemented by derived classes
		void update(CellularMatrix& matrix, int x, int y) override;
		
	protected:
		// Gravitational constant affecting the solid
		const float GRAVITY = 0.1f;

		// Velocity (cells moved per update)
		float velocity_x = 0.0f;
		float velocity_y = 1.0f;
		// Accumulated sub-cell movement for smooth simulation
		float accumulated_x = 0.0f;
		float accumulated_y = 0.0f;
		// Resistance to inertia, affecting how quickly the solid stops
		float inertialResistance = 0.01f;
		// Counter for how many updates the solid has been stopped
		int stoppedMovingCount = 0;
		// Threshold for determining when the solid is considered stopped
		int stoppedMovingThreshold = 5;
		// Flag indicating whether the solid is free falling
		bool isMoving = true;
};

#endif