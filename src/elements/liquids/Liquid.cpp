#include "Liquid.hpp"
#include "CellularMatrix.hpp"

//-------------------------------------------
// Update Behavior
//-------------------------------------------
// Update the state and movement of a movable solid (e.g., sand)
void Liquid::update(CellularMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return; // Prevent double-updating in one frame

	// Lambda: returns true if a cell at (posX, posY) can be replaced (empty or gas)
	auto canReplaceElement = [&](int posX, int posY) -> bool {
		if (!matrix.isInBounds(posX, posY)) return false;
		if (matrix.isEmpty(posX, posY)) return true;
		return false;
	};

	// Lambda: swaps this element with (x2, y2) and marks as moved
	auto swap = [&](int x2, int y2) -> void {
		std::swap(matrix.getElement(x, y), matrix.getElement(x2, y2));
		matrix.getElement(x2, y2)->step = CellularMatrix::step;
	};

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
				// Check for empty neighbors and change color if more than 4 are empty
				int emptyCount = 0;
				for (int dx = -1; dx <= 1; ++dx) {
					for (int dy = -1; dy <= 1; ++dy) {
						if (dx == 0 && dy == 0) continue;
						if (matrix.isInBounds(x + dx, y + dy) && matrix.isEmpty(x + dx, y + dy)) {
							emptyCount++;
						}
					}
				}
				if (emptyCount >= 3) {
					// Temporarily render as empty: override color for this frame
					color = ElementColors::getColorByElementType(EMPTY, x, y);
				}
				else {
					color = ElementColors::getColorByElementType(WATER, x, y);
				}
				// Try to slide diagonally (sand piling)
				if (canReplaceElement(x + 1, y + 1)) {
					swap(x + 1, y + 1);
				}
				else if (canReplaceElement(x - 1, y + 1)) {
					swap(x - 1, y + 1);
				}
				else {
					// Try right first
					int last_valid_x = x;
					for (int i = 1; i <= dispersionRate; ++i) {
						int check_x = x + i;
						if (canReplaceElement(check_x, y)) {
							last_valid_x = check_x;
						} else {
							break;
						}
					}
					if (last_valid_x != x) {
						swap(last_valid_x, y);
						return;
					}
					// Then try left
					last_valid_x = x;
					for (int i = 1; i <= dispersionRate; ++i) {
						int check_x = x - i;
						if (canReplaceElement(check_x, y)) {
							last_valid_x = check_x;
						} else {
							break;
						}
					}
					if (last_valid_x != x) {
						swap(last_valid_x, y);
					}
				}
				// Reset vertical velocity/accumulation if blocked
				velocity_y = 1.0f;
				accumulated_y = 0.0f;
			}
		}
	}
}