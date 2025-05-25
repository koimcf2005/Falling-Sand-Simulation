#include "Element.hpp"
#include "CellularMatrix.hpp"

std::mt19937 Element::rng{std::random_device{}()};

// Helper function to get a random direction (-1 or 1)
int Element::getRandomDirection() {
    static std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng) == 0 ? -1 : 1;
}

void Element::update(CellularMatrix&, int, int) {
	if (step == CellularMatrix::stepped) {
		return;
	}
	step = CellularMatrix::stepped;
}