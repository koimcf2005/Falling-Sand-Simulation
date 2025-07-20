#ifndef DIRT_HPP
#define DIRT_HPP

#include "src/elements/movable/falling/powder/PowderElement.hpp"

class Dirt : public PowderElement {
public:
	Dirt(int x, int y) : PowderElement(ElementType::DIRT, x, y) {
		m_Density = 0.85f;
		m_Friction = 0.2f;
		m_InertialResistance = 0.5f;
	}
};

#endif // DIRT_HPP
