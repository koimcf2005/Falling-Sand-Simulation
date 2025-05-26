// -------------------------------------------
// Liquid.hpp
// -------------------------------------------
// Base class for all liquid elements (e.g., water, lava).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "Element.hpp"

class Liquid : public Element {
	public:
		Liquid(ElementType type, int x, int y) : Element(type, x, y) {}
		// Update function to be implemented by derived classes
		void update(CellularMatrix& matrix, int x, int y) override;
		
	protected:
		// Gravitational constant affecting the solid
		const float GRAVITY = 0.1f;
		// Velocity (cells moved per update)
		float velocity_y = 1.0f;
		float velocity_x = 0.0f;
		// Accumulated sub-cell movement for smooth simulation
		float accumulated_y = 0.0f;
		float accumulated_x = 0.0f;
		// Dispersion Rate
		int dispersionRate = 10;
		// Flag indicating whether the solid is free falling
		bool wasMoving = true;
		bool isMoving = true;
};

#endif