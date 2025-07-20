#include "src/elements/movable/falling/powder/PowderElement.hpp"
#include "src/elements/movable/falling/liquid/LiquidElement.hpp"
#include "src/elements/movable/rising/gas/GasElement.hpp"

bool PowderElement::canSwapWithElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	if (matrix.isEmpty(x, y)) return true;

	auto target = matrix.getElement(x, y);
	if (getType() == target->getType()) return false;

	if (target->as<LiquidElement>()) return true;
	if (target->as<GasElement>()) return true;
	return false;
}

void PowderElement::recieveNeighborEffect() {
	if (!ElementRNG::getRandomChance(m_InertialResistance)) {
		setIsMoving(true);
		return;
	}
}

void PowderElement::handleGrounded(IMatrix& matrix) {
	if (!getIsMoving()) return;

	int dir = ElementRNG::getRandomDirection(); // Randomly pick left or right
	int x1 = m_PosX + dir;
	int x2 = m_PosX - dir;
	int y1 = m_PosY + 1;

	if (ElementRNG::getRandomChance(m_Friction)) {
		setIsMoving(false);
		return;
	}

	// Try to slide diagonally (sand piling)
	if (canSwapWithElement(matrix, x1, y1)) {
		swapWithElement(matrix, x1, y1);
		affectAdjacentNeighbors(matrix);

	} else if (canSwapWithElement(matrix, x2, y1)) {
		swapWithElement(matrix, x2, y1);
		affectAdjacentNeighbors(matrix);
	}
}

void PowderElement::update(IMatrix& matrix) {
	if (checkIfUpdated()) return;
	handleBuoyancy(matrix);
	handleFalling(matrix);
}

void PowderElement::handleBuoyancy(IMatrix& matrix) {
	int x = m_PosX, y = m_PosY;
	auto target = matrix.isInBounds(x, y - 1) ? matrix.getElement(x, y - 1) : nullptr;
	if (auto above = target->as<MovableElement>()) {
		float difference = getDensity() - above->getDensity();
		if (difference < 0) {
			difference = std::fabs(difference);
			if (canSwapWithElement(matrix, x, y - 1)) {
				if (ElementRNG::getRandomChance(difference)) {
					swapWithElement(matrix, x, y - 1);
					setVelocityY(0.0f);
					return;
				}
			}
			int dir = ElementRNG::getRandomDirection();
			if (canSwapWithElement(matrix, x + dir, y)) {
				if (ElementRNG::getRandomChance(difference)) {
					swapWithElement(matrix, x + dir, y);
					setVelocityY(0.0f);
					return;
				}
			}
		}
		else {
			if (canSwapWithElement(matrix, x, y + 1)) {
				if (ElementRNG::getRandomChance(difference)) {
					swapWithElement(matrix, x, y + 1);
					setVelocityY(0.0f);
					return;
				}
			}
		}
	}
}