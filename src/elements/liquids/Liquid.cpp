#include "Liquid.hpp"
#include "CellularMatrix.hpp"
#include "elements/solids/movable/MovableSolid.hpp"

/// ==================== Dissolved Element Accessors ====================
ElementType Liquid::getDissolvedElement() const {
	return dissolvedElement;
}
void Liquid::setDissolvedElement(ElementType type) {
	dissolvedElement = type;
}
void Liquid::setdestroyDissolvedElement(bool value) {
	destroyDissolvedElement = value;
}

// ========== Update ==========
void Liquid::update(IMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	// --- Proper movement boolean handling ---
	wasMoving = isMoving;	// Save last frame's movement state
	isMoving = false;
	movedThisFrame = false;  // Reset movement flag for this frame

	// --- Logic ---
	diffuseDissolvedElement(matrix, x, y);

	handleBuoyancy(matrix, x, y);
	handleFalling(matrix, x, y);

	isMoving = movedThisFrame;
}

// ========== Replace Conditions ==========
bool Liquid::canReplaceElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	Element* elem = matrix.getElement(x, y);
	// Can't replace same type
	if (elem && elem->getType() == getType()) return false;
	
	// Can replace empty cells
	if (matrix.isEmpty(x, y)) return true;
	
	// Can replace other liquids if this liquid is denser
	Liquid* liquid = dynamic_cast<Liquid*>(elem);
	if (liquid) {
		if (liquid->getHasUpdated()) return false;
		if (this->density > liquid->density) {
			return true;
		}
	}
	
	return false;
}

/// ==================== Movement/Spreading Logic ====================
void Liquid::handleHorizontalSpreading(IMatrix& matrix, int x, int y) {
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
	if (canReplaceElement(matrix, x + 1, y + 1)) {
		Element::swapElements(matrix, x, y, x + 1, y + 1);
	} else if (canReplaceElement(matrix, x - 1, y + 1)) {
		Element::swapElements(matrix, x, y, x - 1, y + 1);
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
		Element::swapElements(matrix, x, y, last_valid_x, y);
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
		Element::swapElements(matrix, x, y, last_valid_x, y);
	}
	velocity_y = 1.0f;
	accumulated_y = 0.0f;
}

// ==================== Dissolved Element Diffusion ====================
void Liquid::diffuseDissolvedElement(IMatrix& matrix, int x, int y) {
	// Only attempt to diffuse if there is a dissolved element present
	if (dissolvedElement != EMPTY) {
		// Collect coordinates of neighboring cells that are the same liquid type and have no dissolved element
		std::vector<std::pair<int, int>> neighborCoords;
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				// Skip the center cell (self)
				if (dx == 0 && dy == 0) continue;
				int nx = x + dx;
				int ny = y + dy;
				// Check if neighbor is within bounds
				if (matrix.isInBounds(nx, ny)) {
					Element* elem = matrix.getElement(nx, ny);
					// Only consider neighbors that are the same liquid type and have no dissolved element
					Liquid* neighbor = dynamic_cast<Liquid*>(elem);
					if (neighbor && neighbor->getType() == getType() && neighbor->getDissolvedElement() == EMPTY) {
						neighborCoords.emplace_back(nx, ny);
					}
				}
			}
		}
		// With some probability, pick a random eligible neighbor and swap places
		if (!neighborCoords.empty() && norm_dist(rng) < 0.2f) {
			// Randomly select one of the eligible neighbors
			auto [tx, ty] = neighborCoords[rng() % neighborCoords.size()];
			// Swap this element with the selected neighbor to simulate diffusion
			Element::swapElements(matrix, x, y, tx, ty);
		}
	}
}

// ==================== Buoyancy (Liquid-on-Liquid) ====================
void Liquid::handleBuoyancy(IMatrix& matrix, int x, int y) {
	// Check if a denser liquid is above this one (should sink down)
	if (matrix.isInBounds(x, y - 1)) {
		Element* aboveElem = matrix.getElement(x, y - 1);
		Liquid* above = dynamic_cast<Liquid*>(aboveElem);
		if (above && above->getType() != getType()) {
			if (above->density > this->density && !above->getHasUpdated()) {
				Element::swapElements(matrix, x, y, x, y - 1);
			}
		}
	}
}

// ========== Grounded Effect ==========
void Liquid::handleWhileGrounded(IMatrix& matrix, int x, int y) {
	handleHorizontalSpreading(matrix, x, y);
}

// ========== Neighbor Effect ==========
void Liquid::applyAdjacentNeighborEffect() {
	// Currently no effect from neighbors for liquid types
}