#ifndef COAL_HPP
#define COAL_HPP

#include "src/elements/movable/falling/powder/PowderElement.hpp"

class Coal : public PowderElement {
public:
	Coal(int x, int y) : PowderElement(ElementType::COAL, x, y) {
		m_Density = 0.9f;
		m_Friction = 0.2f;
		m_InertialResistance = 0.3f;
	}
};

#endif // COAL_HPP
