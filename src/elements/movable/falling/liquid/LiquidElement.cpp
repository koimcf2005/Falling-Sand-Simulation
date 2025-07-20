/**
 * @file LiquidElement.cpp
 * @brief Implements LiquidElement logic for liquids with buoyancy and spreading.
 */

#include "src/elements/movable/falling/liquid/LiquidElement.hpp"
#include "src/elements/movable/falling/powder/PowderElement.hpp"

bool LiquidElement::canSwapWithElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	if (matrix.isEmpty(x, y)) return true;

	auto target = matrix.getElement(x, y);
	if (getType() == target->getType()) return false;

	if (auto movable = target->as<MovableElement>()) {
		if (movable->as<PowderElement>()) return false;
		if (getDensity() > movable->getDensity()) return true;
	}
	return false;
}

void LiquidElement::handleHorizontalSpreading(IMatrix& matrix) {
	int x = m_PosX;
	int y = m_PosY;

	// Diagonal slide: try both directions randomly to avoid bias
	int dir = ElementRNG::getRandomDirection(); // Randomly pick left or right
	int x1 = x + dir;
	int x2 = x - dir;
	int y1 = y + 1;

	// Try to slide diagonally (sand piling)
	if (canSwapWithElement(matrix, x1, y1)) {
		swapWithElement(matrix, x1, y1);
		return;
	} else if (canSwapWithElement(matrix, x2, y1)) {
		swapWithElement(matrix, x2, y1);
		return;
	}

	// Randomize horizontal spreading direction for more natural flow
	bool goRightFirst = dir > 0;
	int directions[2] = { goRightFirst ? 1 : -1, goRightFirst ? -1 : 1 };

	// Weighted random: higher probability for farther distances
	int totalWeight = (m_DispersionRate * (m_DispersionRate + 1)) / 2;
	int r = ElementRNG::getRandomInt(1, totalWeight);
	int chosenDistance = 1;
	int acc = 0;
	for (int i = 1; i <= m_DispersionRate; ++i) {
		acc += i;
		if (r <= acc) {
			chosenDistance = i;
			break;
		}
	}

	for (int d = 0; d < 2; ++d) {
		int sign = directions[d];
		int lastValidX = x;
		bool found = false;
		for (int i = 1; i <= chosenDistance; ++i) {
			int checkX = x + sign * i;
			if (canSwapWithElement(matrix, checkX, y)) {
				lastValidX = checkX;
				found = true;
			} else {
				break;
			}
		}
		if (found && lastValidX != x) {
			int lowestY = y;
			while (canSwapWithElement(matrix, lastValidX, lowestY + 1)) {
				++lowestY;
				if (lowestY - y > 20) {
					return;
				}
			}
			swapWithElement(matrix, lastValidX, lowestY);
			return; // Only spread in one direction per update
		}
	}
}

void LiquidElement::update(IMatrix& matrix) {
	if (checkIfUpdated()) return;
	handleBuoyancy(matrix);
	handleFalling(matrix);
}

void LiquidElement::recieveNeighborEffect() {
	return;
}

void LiquidElement::handleGrounded(IMatrix& matrix) {
	handleHorizontalSpreading(matrix);
}

void LiquidElement::handleBuoyancy(IMatrix& matrix) {
	int x = m_PosX, y = m_PosY;
	auto target = matrix.isInBounds(x, y - 1) ? matrix.getElement(x, y - 1) : nullptr;
	if (!target && target->getHasUpdated()) return;
	if (auto above = target->as<MovableElement>()) {
		if (getType() == above->getType()) return;
		if (!above->as<LiquidElement>()) return;
		float difference = getDensity() - above->getDensity();
		if (difference < 0) {
			if (ElementRNG::getRandomChance(std::fabs(difference))) {
				swapWithElement(matrix, x, y - 1);
			}
		}
	}
}