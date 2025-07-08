// -------------------------------------------
// Solid.hpp
// -------------------------------------------
// Base class for all solid elements (movable and immoveable).
// Inherits from Element and provides a friction factor for subclasses.
// Intended for use as a parent for both static and dynamic solids.
// -------------------------------------------
#ifndef SOLID_HPP
#define SOLID_HPP

#include "src/elements/Element.hpp"

/**
 * @brief Base class for all solid elements (movable and immoveable).
 * 
 * Inherits from Element and provides a friction factor for subclasses.
 * Intended for use as a parent for both static and dynamic solids.
 */
class Solid : public Element {
public:
	/// ==================== Construction/Destruction ====================
	Solid(ElementType type, int x, int y) : Element(type, x, y) {}

protected:
	/// ==================== Physical Properties ====================
	float frictionFactor = 0.9f; ///< Friction factor applied to movement (0 = no friction, 1 = full stop)
};

#endif // SOLID_HPP