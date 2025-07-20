#ifndef SALT_HPP
#define SALT_HPP

#include "src/elements/movable/falling/powder/PowderElement.hpp"
#include "src/elements/traits/DissolvableElement.hpp"

class Salt : public PowderElement, DissolvableElement {
public:
	Salt(int x, int y) : PowderElement(ElementType::SALT, x, y) {
		m_Density = 0.2f;
		m_Solvents = {
			{WATER, 0.005f}
		};
	}
	void update(IMatrix& matrix) override {
		PowderElement::update(matrix);
		handleDissolving(matrix, m_PosX, m_PosY);
	}
};

#endif // SALT_HPP
