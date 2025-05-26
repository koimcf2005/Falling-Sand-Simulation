#include "Element.hpp"
#include "CellularMatrix.hpp"

std::mt19937 Element::rng{std::random_device{}()};

// Helper function to get a random direction (-1 or 1)
int Element::getRandomDirection() {
    static std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng) == 0 ? -1 : 1;
}

bool Element::checkIfUpdated() {
	if (step == CellularMatrix::step) {
		return true;
	}
	step = CellularMatrix::step;
	return false;
}

void Element::destroyElement(CellularMatrix& matrix, int x, int y) {
	if (matrix.getElement(x, y)->getType() != EMPTY) {
		Element* emptyElement = createElementFromType(EMPTY, x, y);
		Element* oldElement = matrix.matrix[y][x];
		matrix.matrix[y][x] = emptyElement;
		delete oldElement;
	}
}

void Element::update(CellularMatrix&, int, int) {
	return;
}