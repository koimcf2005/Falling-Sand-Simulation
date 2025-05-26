#include "Gas.hpp"
#include "elements/liquids/Liquid.hpp"
#include "CellularMatrix.hpp"

//-------------------------------------------
// Update Behavior
//-------------------------------------------
void Gas::update(CellularMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	// Lambda: returns true if a cell at (posX, posY) can be replaced (empty or gas)
	auto canReplaceElement = [&](int posX, int posY) -> bool {
		if (!matrix.isInBounds(posX, posY)) return false;
		if (matrix.isEmpty(posX, posY)) return true;
		if (Element::isInstanceOf<Liquid>(matrix.getElement(posX, posY))) return true;
		return false;
	};

	std::uniform_real_distribution<float> dist(0, 1);
	if (dist(rng) < chanceOfDeath) destroyElement(matrix, x, y);

	// Fallback: Try to fall straight up by 1 cell
	if (dist(rng) > chanceOfHorizontal && canReplaceElement(x, y - 1)) {
		std::swap(matrix.getElement(x, y), matrix.getElement(x, y - 1));
	}
	else {
		int dir = getRandomDirection();
		// Try falling diagonally to the bottom-left or right (randomized)
		if (canReplaceElement(x + dir, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x + dir, y - 1));
		}
		else if (canReplaceElement(x - dir, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x - dir, y - 1));
		}
		else if (canReplaceElement(x - dir, y)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x - dir, y));
		}
		else if (canReplaceElement(x + dir, y)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x + dir, y));
		}
		else if (canReplaceElement(x, y - 1)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(x, y - 1));
		}
	}
}