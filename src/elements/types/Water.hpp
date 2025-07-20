#ifndef WATER_HPP
#define WATER_HPP

#include "src/elements/movable/falling/liquid/LiquidElement.hpp"
#include "src/elements/traits/SolvantElement.hpp"

class Water : public LiquidElement, SolvantElement {
public:
	Water(int x, int y) : LiquidElement(ElementType::WATER, x, y) {
		m_Density = 0.5f;
		m_DissolvedElement = EMPTY;
	}
};

#endif // WATER_HPP
