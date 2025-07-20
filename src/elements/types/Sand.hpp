#ifndef SAND_HPP
#define SAND_HPP

#include "src/elements/movable/falling/powder/PowderElement.hpp"

class Sand : public PowderElement {
public:
	Sand(int x, int y) : PowderElement(ElementType::SAND, x, y) {
		m_Density = 0.8f;
		m_Friction = 0.035f;
		m_InertialResistance = 0.0f;
	}
};

#endif // SAND_HPP
