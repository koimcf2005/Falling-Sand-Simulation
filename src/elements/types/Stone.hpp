#ifndef STONE_HPP
#define STONE_HPP

#include "src/elements/static/StaticElement.hpp"

class Stone : public StaticElement {
public:
	Stone(int x, int y) : StaticElement(ElementType::STONE, x, y) {}
};

#endif // STONE_HPP
