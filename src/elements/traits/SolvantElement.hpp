#ifndef SOLVANT_ELEMENT_HPP
#define SOLVANT_ELEMENT_HPP

#include "src/core/IMatrix.hpp"

class SolvantElement {
public:
	virtual ~SolvantElement() = default; // Ensure polymorphic behavior

	ElementType getDissolvedElement() const;
	void setDissolvedElement(ElementType type);

protected:
	ElementType m_DissolvedElement { EMPTY };
};

#endif // SOLVANT_ELEMENT_HPP
