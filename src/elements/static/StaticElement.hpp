#ifndef STATIC_ELEMENT_HPP
#define STATIC_ELEMENT_HPP

#include "src/elements/Element.hpp"

/**
 * @brief Represents a static (immovable) element in the simulation.
 * 
 * Static elements do not move or react to gravity/buoyancy.
 */
class StaticElement : public Element {
public:
	StaticElement(ElementType type, int x, int y) : Element(type, x, y) {}

	/**
	 * @brief Update method for static elements (does nothing).
	 * @param matrix The simulation matrix.
	 */
	void update(IMatrix& /*matrix*/) override {
		// Static elements do not update/move.
	}
};

#endif // STATIC_ELEMENT_HPP
