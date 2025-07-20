#ifndef SMOKE_HPP
#define SMOKE_HPP

#include "src/elements/movable/rising/gas/GasElement.hpp"

class Smoke : public GasElement {
public:
	Smoke(int x, int y) : GasElement(ElementType::SMOKE, x, y) {
		m_Density = 0.05f;
	}
};

#endif // SMOKE_HPP
