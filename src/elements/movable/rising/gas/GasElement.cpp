#include "src/elements/movable/rising/gas/GasElement.hpp"

bool GasElement::canSwapWithElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	Element* target = matrix.getElement(x, y);
	if (!target) return matrix.isEmpty(x, y);
	// Can't replace updated targets
	if (target->getHasUpdated()) return false;
	// Can't replace same type
	if (target->getType() == getType()) return false;
	// Can replace empty cells
	if (matrix.isEmpty(x, y)) return true;
	return false;
}

void GasElement::recieveNeighborEffect() {
	// Gases are easily disturbed by neighbors, so mark as moving
	setIsMoving(true);
}

void GasElement::handleCeilinged(IMatrix& matrix) {
	// When ceilinged, try to spread horizontally (left or right)
	int left = m_PosX - 1;
	int right = m_PosX + 1;
	bool moved = false;

	if (canSwapWithElement(matrix, left, m_PosY)) {
		swapWithElement(matrix, left, m_PosY);
		moved = true;
	} else if (canSwapWithElement(matrix, right, m_PosY)) {
		swapWithElement(matrix, right, m_PosY);
		moved = true;
	}

	if (!moved) {
		// If can't move, remain stationary
		setIsMoving(false);
	}
}

void GasElement::update(IMatrix& matrix) {
	if (checkIfUpdated()) return;
	matrix.activateChunk(m_PosX, m_PosY);

	m_TimeUntilDeath--;
	if (m_TimeUntilDeath < 0) {
		if (ElementRNG::getRandomChance(m_ChanceOfDeathAfterTimer)) {
			destroySelf(matrix);
			return;
		}
	}

	handleRising(matrix);
}