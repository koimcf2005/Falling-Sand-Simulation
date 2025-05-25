#ifndef STONE_HPP
#define STONE_HPP

#include "ImmoveableSolid.hpp"

class Stone : public ImmoveableSolid {
public:
	Stone(int x, int y) : ImmoveableSolid(STONE, x, y) {}
};

#endif // STONE_HPP
