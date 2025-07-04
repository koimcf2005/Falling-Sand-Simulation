#ifndef STEAM_HPP
#define STEAM_HPP

#include "Gas.hpp"

class Steam : public Gas {
public:
	Steam(int x, int y) : Gas(STEAM, x, y) {
		chanceOfHorizontal = 0.1f;
		chanceOfDeath = 0.0075f;
	}
};

#endif // STEAM_HPP
