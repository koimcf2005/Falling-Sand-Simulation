#include "MovableSolid.hpp"
#include "CellularMatrix.hpp"
#include "elements/gases/Gas.hpp"
#include "elements/liquids/Liquid.hpp"


//-------------------------------------------
// Update Behavior
//-------------------------------------------
// Update the state and movement of a movable solid (e.g., sand)
void MovableSolid::update(CellularMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return; // Prevent double-updating in one frame

	bool movedThisFrame = false; // Tracks if this particle moved this frame

	// Lambda: returns true if a cell at (posX, posY) can be replaced (empty or gas)
	auto canReplaceElement = [&](int posX, int posY) -> bool {
		if (!matrix.isInBounds(posX, posY)) return false;
		if (matrix.isEmpty(posX, posY)) return true;
		if (Element::isInstanceOf<Gas>(matrix.getElement(posX, posY))) return true;
		if (Element::isInstanceOf<Liquid>(matrix.getElement(posX, posY))) return true;
		return false;
	};

	// Lambda: swaps this element with (x2, y2) and marks as moved
	auto swap = [&](int x2, int y2) -> void {
		std::swap(matrix.getElement(x, y), matrix.getElement(x2, y2));
		matrix.getElement(x2, y2)->step = CellularMatrix::step;
		movedThisFrame = true;
	};

	// If the cell below is empty or gas, particle is considered moving
	if (canReplaceElement(x, y + 1)) isMoving = true;
	if (matrix.isInBounds(x, y + 1) && Element::isInstanceOf<Liquid>(matrix.getElement(x, y + 1))) {
		velocity_y = 0.5f;
	}

	if (isMoving) {
		// Apply gravity and accumulate vertical velocity
		velocity_y = std::clamp(velocity_y + GRAVITY, 0.0f, 10.0f);
		accumulated_y += velocity_y;
		int move_y = static_cast<int>(accumulated_y);

		// Try to move vertically as far as possible
		if (move_y != 0) {
			int last_valid_y = y;
			for (int i = 1; i <= std::abs(move_y); ++i) {
				int check_y = y + (move_y > 0 ? i : -i);
				if (canReplaceElement(x, check_y)) {
					last_valid_y = check_y;
				} else {
					break;
				}
			}
			if (last_valid_y != y) {
				// Move vertically
				swap(x, last_valid_y);
				accumulated_y -= (last_valid_y - y);
			} else {
				// Blocked vertically: try to slide or move horizontally
				int dir = getRandomDirection(); // Randomly pick left or right
				// If falling fast enough, set horizontal velocity
				if (velocity_y >= 1.0f / absorption) {
					velocity_x = velocity_y * absorption * dir;
				}
				// Try to slide diagonally (sand piling)
				if (canReplaceElement(x + dir, y + 1)) {
					swap(x + dir, y + 1);
					velocity_x += 0.5f * dir;
				}
				else if (canReplaceElement(x - dir, y + 1)) {
					swap(x - dir, y + 1);
					velocity_x += 0.5f * dir;
				}
				// Try to move horizontally as far as possible using velocity_x
				int horiz_dir = (velocity_x > 0) ? 1 : (velocity_x < 0) ? -1 : 0;
				int max_steps = std::abs(static_cast<int>(velocity_x));
				int last_valid_x = x;
				for (int i = 1; i <= max_steps; ++i) {
					int target_x = x + i * horiz_dir;
					if (canReplaceElement(target_x, y)) {
						last_valid_x = target_x;
					} else {
						break;
					}
				}
				if (last_valid_x != x) {
					swap(last_valid_x, y);
					velocity_x *= friction;
				}
				// Reset vertical velocity/accumulation if blocked
				velocity_y = 1.0f;
				accumulated_y = 0.0f;
			}
		}

		// Inertia propagation: randomly wake up left/right neighbors
		MovableSolid* leftNeighbor = nullptr;
		if (matrix.isInBounds(x - 1, y))
			leftNeighbor = dynamic_cast<MovableSolid*>(matrix.getElement(x - 1, y));
		MovableSolid* rightNeighbor = nullptr;
		if (matrix.isInBounds(x + 1, y))
			rightNeighbor = dynamic_cast<MovableSolid*>(matrix.getElement(x + 1, y));
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		if (leftNeighbor != nullptr && dist(rng) > leftNeighbor->inertialResistance) leftNeighbor->isMoving = true;
		if (rightNeighbor != nullptr && dist(rng) > rightNeighbor->inertialResistance) rightNeighbor->isMoving = true;

		// If not moved for two frames, stop moving
		if (!movedThisFrame && !wasMoving) isMoving = false;
		wasMoving = movedThisFrame;
	}
}