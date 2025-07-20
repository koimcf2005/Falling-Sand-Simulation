#ifndef ASH_HPP
#define ASH_HPP

#include "src/elements/movable/falling/powder/PowderElement.hpp"

class Ash : public PowderElement {
public:
	Ash(int x, int y) : PowderElement(ElementType::ASH, x, y) {
		m_Density = 0.1f;
	}
};

#endif // ASH_HPP
