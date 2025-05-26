#ifndef WOOD_HPP
#define WOOD_HPP

#include "ImmoveableSolid.hpp"

class Wood : public ImmoveableSolid {
public:
	Wood(int x, int y) : ImmoveableSolid(WOOD, x, y) {}
};

#endif // WOOD_HPP
