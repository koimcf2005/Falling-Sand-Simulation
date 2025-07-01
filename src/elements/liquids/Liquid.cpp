#include "Liquid.hpp"
#include "CellularMatrix.hpp"
#include "elements/solids/movable/MovableSolid.hpp"

//-------------------------------------------
// Dissolved Element Accessors
//-------------------------------------------
ElementType Liquid::getDissolvedElement() const {
	return dissolvedElement;
}
void Liquid::setDissolvedElement(ElementType type) {
	dissolvedElement = type;
}
void Liquid::setdestroyDissolvedElement(bool value) {
	destroyDissolvedElement = value;
}

//-------------------------------------------
// Dissolved Element Diffusion
//-------------------------------------------
void Liquid::diffuseDissolvedElement(IMatrixAccess& matrix, int x, int y) {
	if (dissolvedElement != EMPTY) {
		std::vector<std::pair<int, int>> neighborCoords;
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 && dy == 0) continue;
				int nx = x + dx;
				int ny = y + dy;
				if (matrix.isInBounds(nx, ny)) {
					Element* elem = matrix.getElement(nx, ny);
					Liquid* neighbor = dynamic_cast<Liquid*>(elem);
					if (neighbor && neighbor->getType() == getType() && neighbor->getDissolvedElement() == EMPTY) {
						neighborCoords.emplace_back(nx, ny);
					}
				}
			}
		}
		if (!neighborCoords.empty() && norm_dist(rng) < 0.2f) {
			auto [tx, ty] = neighborCoords[rng() % neighborCoords.size()];
			Element::swapElement(matrix, x, y, tx, ty);
		}
	}
}

//-------------------------------------------
// Movement/Spreading Logic
//-------------------------------------------
bool Liquid::tryVerticalMove(IMatrixAccess& matrix, int x, int y, int move_y) {
	int last_valid_y = y;
	for (int i = 1; i <= std::abs(move_y); ++i) {
		int check_y = y + (move_y > 0 ? i : -i);
		if (canReplaceElementForLiquid(matrix, x, check_y)) {
			last_valid_y = check_y;
		} else {
			break;
		}
	}
	if (last_valid_y != y) {
		Element::swapElement(matrix, x, y, x, last_valid_y);
		accumulated_y -= (last_valid_y - y);
		return true;
	}
	return false;
}

bool Liquid::handleHorizontalSpreading(IMatrixAccess& matrix, int x, int y) {
	bool moved = false;
	int emptyCount = 0;
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			if (matrix.isInBounds(x + dx, y + dy) && matrix.isEmpty(x + dx, y + dy)) {
				emptyCount++;
			}
		}
	}
	// Diagonal slide
	if (canReplaceElementForLiquid(matrix, x + 1, y + 1)) {
		Element::swapElement(matrix, x, y, x + 1, y + 1);
		moved = true;
	} else if (canReplaceElementForLiquid(matrix, x - 1, y + 1)) {
		Element::swapElement(matrix, x, y, x - 1, y + 1);
		moved = true;
	}
	// Right
	int last_valid_x = x;
	for (int i = 1; i <= dispersionRate; ++i) {
		int check_x = x + i;
		if (!matrix.isInBounds(check_x, y)) break;
		if (matrix.isEmpty(check_x, y)) {
			last_valid_x = check_x;
		} else {
			Element* elem = matrix.getElement(check_x, y);
			Liquid* other = dynamic_cast<Liquid*>(elem);
			if (other && other->getType() != getType() && this->density > other->density) {
				last_valid_x = check_x;
			} else {
				break;
			}
		}
	}
	if (last_valid_x != x) {
		Element::swapElement(matrix, x, y, last_valid_x, y);
		moved = true;
	}
	// Left
	last_valid_x = x;
	for (int i = 1; i <= dispersionRate; ++i) {
		int check_x = x - i;
		if (!matrix.isInBounds(check_x, y)) break;
		if (matrix.isEmpty(check_x, y)) {
			last_valid_x = check_x;
		} else {
			Element* elem = matrix.getElement(check_x, y);
			Liquid* other = dynamic_cast<Liquid*>(elem);
			if (other && other->getType() != getType() && this->density > other->density) {
				last_valid_x = check_x;
			} else {
				break;
			}
		}
	}
	if (last_valid_x != x) {
		Element::swapElement(matrix, x, y, last_valid_x, y);
		moved = true;
	}
	velocity_y = 1.0f;
	accumulated_y = 0.0f;
	return moved;
}

//-------------------------------------------
// Buoyancy (Liquid-on-Liquid)
//-------------------------------------------
bool Liquid::handleLiquidBuoyancy(IMatrixAccess& matrix, int x, int y) {
	if (matrix.isInBounds(x, y - 1)) {
		Element* aboveElem = matrix.getElement(x, y - 1);
		Liquid* above = dynamic_cast<Liquid*>(aboveElem);
		if (above && above->getType() != getType()) {
			if (this->density < above->density) {
				Element::swapElement(matrix, x, y, x, y - 1);
				return true;
			}
		}
	}
	if (matrix.isInBounds(x, y + 1)) {
		Element* belowElem = matrix.getElement(x, y + 1);
		Liquid* below = dynamic_cast<Liquid*>(belowElem);
		if (below && below->getType() != getType()) {
			if (this->density > below->density) {
				Element::swapElement(matrix, x, y, x, y + 1);
				return true;
			}
		}
	}
	return false;
}

//-------------------------------------------
// Inertia Propagation
//-------------------------------------------
void Liquid::propagateInertiaToNeighbors(IMatrixAccess& matrix, int x, int y) {
	MovableSolid* leftNeighbor = nullptr;
	if (matrix.isInBounds(x - 1, y))
		leftNeighbor = dynamic_cast<MovableSolid*>(matrix.getElement(x - 1, y));
	MovableSolid* rightNeighbor = nullptr;
	if (matrix.isInBounds(x + 1, y))
		rightNeighbor = dynamic_cast<MovableSolid*>(matrix.getElement(x + 1, y));
	if (leftNeighbor != nullptr) leftNeighbor->setMovingByInertia();
	if (rightNeighbor != nullptr) rightNeighbor->setMovingByInertia();
}

//-------------------------------------------
// Main Update
//-------------------------------------------
bool Liquid::canReplaceElementForLiquid(IMatrixAccess& matrix, int posX, int posY) const {
	if (!matrix.isInBounds(posX, posY)) return false;
	Element* elem = matrix.getElement(posX, posY);
	if (elem && elem->getType() == getType()) return false;
	if (matrix.isEmpty(posX, posY)) return true;
	return false;
}

void Liquid::update(IMatrixAccess& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	diffuseDissolvedElement(matrix, x, y);

	bool movedThisFrame = false;

	// Only set isMoving to true if there is a reason to start moving
	if (!isMoving) {
		// Start moving if the cell below is empty or if horizontal movement is possible
		if (canReplaceElementForLiquid(matrix, x, y + 1)
			|| canReplaceElementForLiquid(matrix, x + 1, y)
			|| canReplaceElementForLiquid(matrix, x - 1, y)) {
			isMoving = true;
		}
	}

	if (isMoving) {
		velocity_y = std::clamp(velocity_y + GRAVITY, 0.0f, 10.0f);
		accumulated_y += velocity_y;
		int move_y = static_cast<int>(accumulated_y);

		if (move_y != 0) {
			if (tryVerticalMove(matrix, x, y, move_y)) {
				movedThisFrame = true;
				return;
			} else {
				movedThisFrame = handleHorizontalSpreading(matrix, x, y);

			}
		}

		if (handleLiquidBuoyancy(matrix, x, y)) {
			movedThisFrame = true;
			return;
		}

		// If not moved for two frames, stop moving
		if (!movedThisFrame && !wasMoving) isMoving = false;
		wasMoving = movedThisFrame;

		propagateInertiaToNeighbors(matrix, x, y);
	}
}