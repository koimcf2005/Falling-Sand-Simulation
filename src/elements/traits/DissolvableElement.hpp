#ifndef DISSOLVABLE_ELEMENT_HPP
#define DISSOLVABLE_ELEMENT_HPP

#include <unordered_map>

#include "src/core/IMatrix.hpp"

class DissolvableElement {
protected:
	void handleDissolving(IMatrix& matrix, int x, int y);
	std::unordered_map<ElementType, float> m_Solvents;
};

#endif // DISSOLVABLE_ELEMENT_HPP
