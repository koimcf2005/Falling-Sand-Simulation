#include "Gas.hpp"
#include "CellularMatrix.hpp"

//-------------------------------------------
// Update Behavior
//-------------------------------------------
void Gas::update(CellularMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	std::uniform_real_distribution<float> dist(0, 1);
	if (dist(rng) < chanceOfDeath) destroyElement(matrix, x, y);

	// Fallback: Try to fall straight up by 1 cell
	if (dist(rng) > chanceOfHorizontal && matrix.isInBounds(x, y - 1) && matrix.isEmpty(x, y - 1)) {
		std::swap(matrix.getElement(x, y), matrix.getElement(x, y - 1));
	}
	else {
		int dir = getRandomDirection();
		// Try falling diagonally to the bottom-left or right (randomized)
		if (matrix.isInBounds(x + dir, y - 1) && matrix.isEmpty(x + dir, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x + dir, y - 1));
		}
		else if (matrix.isInBounds(x - dir, y - 1) && matrix.isEmpty(x - dir, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x - dir, y - 1));
		}
		else if (matrix.isInBounds(x - dir, y) && matrix.isEmpty(x - dir, y)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x - dir, y));
		}
		else if (matrix.isInBounds(x + dir, y) && matrix.isEmpty(x + dir, y)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x + dir, y));
		}
		else if (matrix.isInBounds(x, y - 1) && matrix.isEmpty(x, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x, y - 1));
		}
	}
}