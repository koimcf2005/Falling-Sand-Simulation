#include "MovableSolid.hpp"
#include "CellularMatrix.hpp"

//-------------------------------------------
// Update Behavior
//-------------------------------------------
void MovableSolid::update(CellularMatrix& matrix, int x, int y) {
	Element::update(matrix, x, y);
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	// If the cell directly below is not empty, then stop vertical velocity and add bounce
	if (!matrix.isInBounds(x, y + 1) || (matrix.isInBounds(x, y + 1) && !matrix.isEmpty(x, y + 1))) {
		velocity_x = getRandomDirection() == -1 ? -velocity_y * 0.195f : velocity_y * 0.195f;
		// Apply friction to horizontal velocity when on the ground
		velocity_x *= 0.9f; // Friction coefficient (tweak as needed)
		if (std::abs(velocity_x) < 0.05f) velocity_x = 0.0f; // Stop if very slow
		velocity_y = 0;
		accumulated_y = 0.0f; // Reset accumulated_y for smooth bounce
	}
	else {
		// Apply gravity to vertical velocity and clamp it within a reasonable range
		velocity_y = std::clamp(velocity_y + GRAVITY, -10.0f, 10.0f);
	}

	// Accumulate velocity for smooth movement
	accumulated_x += velocity_x;
	accumulated_y += velocity_y;

	// Determine integer movement from accumulated values
	int move_x = static_cast<int>(accumulated_x);
	int move_y = static_cast<int>(accumulated_y);

	// Only move if at least one cell boundary is crossed
	if (move_x != 0 || move_y != 0) {
		int target_x = x + move_x;
		int target_y = y + move_y;
		if (matrix.isInBounds(target_x, target_y) && matrix.isEmpty(target_x, target_y)) {
			std::swap(matrix.getElement(x, y), matrix.getElement(target_x, target_y));
			// Subtract the integer part that was used for movement
			accumulated_x -= move_x;
			accumulated_y -= move_y;
			return;
		}
	}
	// Fallback: Try to fall straight down by 1 cell
	if (matrix.isInBounds(x, y + 1) && matrix.isEmpty(x, y + 1)) {
		std::swap(matrix.getElement(x, y), matrix.getElement(x, y + 1));
		isMoving = true;
		accumulated_x = 0.0f;
		accumulated_y = 0.0f;
		// Chance to make immediate left and right neighbors moving
		if (matrix.isInBounds(x - 1, y)) {
			Element* left = matrix.getElement(x - 1, y);
			if (auto* mov = dynamic_cast<MovableSolid*>(left)) {
				if (dist(rng) > mov->inertialResistance) {
					mov->isMoving = true;
				}
			}
		}
		if (matrix.isInBounds(x + 1, y)) {
			Element* right = matrix.getElement(x + 1, y);
			if (auto* mov = dynamic_cast<MovableSolid*>(right)) {
				if (dist(rng) > mov->inertialResistance) {
					mov->isMoving = true;
				}
			}
		}
	}
	else {
		isMoving = inertialResistance < dist(rng) && isMoving ? true : false;
		if (isMoving) {
			int dir = getRandomDirection();
			// Try falling diagonally to the bottom-left or right (randomized)
			if (matrix.isInBounds(x + dir, y + 1) && matrix.isEmpty(x + dir, y + 1)) {
				std::swap(matrix.getElement(x, y), matrix.getElement(x + dir, y + 1));
				accumulated_x = 0.0f;
				accumulated_y = 0.0f;
			}
			else if (matrix.isInBounds(x - dir, y + 1) && matrix.isEmpty(x - dir, y + 1)) {
				std::swap(matrix.getElement(x, y), matrix.getElement(x - dir, y + 1));
				accumulated_x = 0.0f;
				accumulated_y = 0.0f;
			}
			else {
				// Fully blocked: stop vertical velocity and reset accumulated_y
				velocity_y = 0.0f;
				accumulated_y = 0.0f;
			}
		}

	}
}