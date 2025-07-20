#ifndef STEAM_HPP
#define STEAM_HPP

#include "src/elements/movable/rising/gas/GasElement.hpp"

class Steam : public GasElement {
public:
	Steam(int x, int y) : GasElement(ElementType::STEAM, x, y) {
		m_Density = 0.02f;
	}
};

#endif // STEAM_HPP
