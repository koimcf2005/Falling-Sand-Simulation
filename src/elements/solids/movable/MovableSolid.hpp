// -------------------------------------------
// MovableSolid.hpp
// -------------------------------------------
// Base class for all movable solid elements (e.g., sand, powder).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef MOVABLE_SOLID_HPP
#define MOVABLE_SOLID_HPP

#include <unordered_map>
#include "Solid.hpp"

class MovableSolid : public Solid {
	public:
		MovableSolid(ElementType type, int x, int y) : Solid(type, x, y) {}
		// Update function to be implemented by derived classes
		void update(IMatrixAccess& matrix, int x, int y) override;

		void setMovingByInertia();

	protected:
		bool canReplaceElement(IMatrixAccess& matrix, int x, int y) const;

		bool handleDissolving(IMatrixAccess& matrix, int x, int y);

		void handleDensity(IMatrixAccess& matrix, int x, int y);

		void affectAdjacentNeighbors(IMatrixAccess& matrix, int x, int y);

		// Gravitational constant affecting the solid
		const float GRAVITY = 0.1f;
		// Velocity (cells moved per update)
		float velocity_y = 1.0f;
		float velocity_x = 0.0f;
		// Accumulated sub-cell movement for smooth simulation
		float accumulated_y = 0.0f;
		float accumulated_x = 0.0f;
		// Friction
		float friction = 0.5f;
		// Absorption
		float absorption = 0.5f;
		// Resistance to inertia, affecting how quickly the solid stops
		float inertialResistance = 0.5f;
};

#endif