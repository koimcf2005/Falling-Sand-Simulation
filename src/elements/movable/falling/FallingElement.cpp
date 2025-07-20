#include "src/elements/movable/falling/FallingElement.hpp"

void FallingElement::handleFalling(IMatrix& matrix) {
	int x = m_PosX;
	int y = m_PosY;
	// Check if the space directly below can be swapped into (i.e., is empty or can be moved into)
	if (canSwapWithElement(matrix, x, y + 1)) {
		m_IsMoving = true; // Mark this element as currently moving

		// Apply gravitational force to vertical velocity
		addVelocityY(GRAVITY); // Increase vertical velocity by gravity constant

		// Accumulate vertical motion for sub-pixel movement
		m_AccumulatedY += getVelocityY();

		// Calculate how many integer rows we are ready to fall (truncate to int)
		int deltaY = static_cast<int>(m_AccumulatedY);

		if (deltaY != 0) {
			int lastValidY = y; // Track the furthest valid Y position we can move to

			// Try to fall as far as allowed by collision rules, up to deltaY rows
			for (int i = 1; i <= std::abs(deltaY); ++i) {
				int checkY = y + (deltaY > 0 ? i : -i); // Check each row in the direction of movement

				// If we can swap with the element at (x, checkY), update lastValidY
				if (canSwapWithElement(matrix, x, checkY)) {
					lastValidY = checkY;
				} else {
					break; // Stop if we hit an obstacle
				}
			}

			// Only perform the swap if we actually moved to a new row
			if (lastValidY != y) {
				// If there is a movable element above, transfer our vertical velocity to it
				if (matrix.isInBounds(x, y - 1)) {
					if (auto movable = matrix.getElement(x, y - 1)->as<MovableElement>()) {
						movable->setVelocityY(getVelocityY());
					}
				}
				// Swap this element with the one at the new position
				swapWithElement(matrix, x, lastValidY);
				// Optionally affect adjacent neighbors (e.g., for sand spreading)
				affectAdjacentNeighbors(matrix);

				// Remove the moved portion from the accumulator (keep the fractional part)
				m_AccumulatedY -= (lastValidY - y);
			}
		}
	} else {
		// If we can't fall, reset vertical velocity and accumulator
		setVelocityY(0.0f);
		m_AccumulatedY = 0.0f;
		// Call subclass-defined behavior for grounded state (e.g., sand settling)
		handleGrounded(matrix);
	}
}