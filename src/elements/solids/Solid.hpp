// -------------------------------------------
// Solid.hpp
// -------------------------------------------
// Base class for all solid elements (movable and immoveable).
// Inherits from Element and provides a friction factor for subclasses.
// Intended for use as a parent for both static and dynamic solids.
// -------------------------------------------
#ifndef SOLID_HPP
#define SOLID_HPP

#include "Element.hpp"

class Solid : public Element {
	public:
		Solid(ElementType type, int x, int y) : Element(type, x, y) {}

	protected:
		// Friction factor applied to movement (0 = no friction, 1 = full stop)
		float frictionFactor = 0.9f;
};

#endif