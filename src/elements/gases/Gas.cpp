#include "Gas.hpp"
#include "elements/liquids/Liquid.hpp"
#include "CellularMatrix.hpp"

/// ==================== Update ====================
void Gas::update(IMatrix& matrix, int x, int y) {
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

	// Fallback: Try to move straight up by 1 cell
	if (dist(rng) > chanceOfHorizontal && canReplaceElement(x, y - 1)) {
		Element::swapElements(matrix, x, y, x, y - 1);
	}
	else {
		int dir = getRandomDirection();
		// Try moving diagonally up-left or up-right (randomized)
		if (canReplaceElement(x + dir, y - 1)) {
			Element::swapElements(matrix, x, y, x + dir, y - 1);
		}
		else if (canReplaceElement(x - dir, y - 1)) {
			Element::swapElements(matrix, x, y, x - dir, y - 1);
		}
		else if (canReplaceElement(x - dir, y)) {
			Element::swapElements(matrix, x, y, x - dir, y);
		}
		else if (canReplaceElement(x + dir, y)) {
			Element::swapElements(matrix, x, y, x + dir, y);
		}
		else if (canReplaceElement(x, y - 1)) {
			Element::swapElements(matrix, x, y, x, y - 1);
		}
	}
}