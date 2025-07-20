#ifndef EMPTY_HPP
#define EMPTY_HPP

#include "src/elements/static/StaticElement.hpp"

class Empty : public StaticElement {
public:
	Empty(int x, int y) : StaticElement(ElementType::EMPTY, x, y) {}
};

#endif // EMPTY_HPP
