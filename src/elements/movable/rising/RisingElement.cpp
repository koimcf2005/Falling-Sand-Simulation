#include "src/elements/movable/rising/RisingElement.hpp"

void RisingElement::handleRising(IMatrix& matrix) {
	int x = m_PosX, y = m_PosY;
	if (!ElementRNG::getRandomChance(m_ChanceOfHorizontal)) {
		if (canSwapWithElement(matrix, x, y - 1)) {
			swapWithElement(matrix, x, y - 1);
			return;
		}
	}
	else {
		int direction = ElementRNG::getRandomDirection();
		
		if (canSwapWithElement(matrix, x + direction, y - 1)) {
			swapWithElement(matrix, x + direction, y - 1);
			return;
		}
		else if (canSwapWithElement(matrix, x - direction, y - 1)) {
			swapWithElement(matrix, x - direction, y - 1);
			return;
		}
		else if (canSwapWithElement(matrix, x + direction, y)) {
			swapWithElement(matrix, x + direction, y);
			return;
		}
		else if (canSwapWithElement(matrix, x - direction, y)) {
			swapWithElement(matrix, x - direction, y);
			return;
		}
	}

	// Can't rise further or no upward movement
	m_VelocityY = 0.0f;
	m_AccumulatedY = 0.0f;
	setIsMoving(false);
	handleCeilinged(matrix);
}
