#ifndef OIL_HPP
#define OIL_HPP

#include "src/elements/movable/falling/liquid/LiquidElement.hpp"

class Oil : public LiquidElement {
public:
	Oil(int x, int y) : LiquidElement(ElementType::OIL, x, y) {
		m_Density = 0.4f;
	}
};

#endif // OIL_HPP
