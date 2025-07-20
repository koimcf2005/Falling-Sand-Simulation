#ifndef WOOD_HPP
#define WOOD_HPP

#include "src/elements/static/StaticElement.hpp"

class Wood : public StaticElement {
public:
	Wood(int x, int y) : StaticElement(ElementType::WOOD, x, y) {}
};

#endif // WOOD_HPP
