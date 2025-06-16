// ------------------------------------------
// ImmoveableSolid.hpp
// ------------------------------------------
// Abstract base class for all immoveable (static) solid elements in the simulation.
// Inherit from this for solids that do not move (e.g., Stone).
// This class does not add new members or methods, but serves as a marker for static solids.
// ------------------------------------------
#ifndef IMMOVEABLE_SOLID_HPP
#define IMMOVEABLE_SOLID_HPP

#include "Solid.hpp"

class ImmoveableSolid : public Solid {
public:
	ImmoveableSolid(ElementType type, int x, int y) : Solid(type, x, y) {}
	// No-op update: does not move or change
	void update(IMatrixAccess& matrix, int x, int y) override;
protected:
	// No additional members
};

#endif // IMMOVEABLE_SOLID_HPP