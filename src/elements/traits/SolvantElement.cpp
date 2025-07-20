#include "src/elements/traits/SolvantElement.hpp"

ElementType SolvantElement::getDissolvedElement() const {
	return m_DissolvedElement;
}

void SolvantElement::setDissolvedElement(ElementType type) {
	m_DissolvedElement = type;
}