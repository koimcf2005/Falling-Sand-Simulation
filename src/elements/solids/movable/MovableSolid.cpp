// src/elements/solids/movable/MovableSolid.cpp
#include "MovableSolid.hpp"
#include "CellularMatrix.hpp"
#include "elements/gases/Gas.hpp"
#include "elements/liquids/Liquid.hpp"

// ========= Update Loop =========
void MovableSolid::update(IMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	// Backup last frame's movement status
	wasMoving = isMoving;
	movedThisFrame = false;

	if (getShouldDissolve(matrix, x, y)) {
		Element::destroyElement(matrix, x, y);
		return;
	}

	handleBuoyancy(matrix, x, y);

	handleFalling(matrix, x, y);

	isMoving = movedThisFrame;

	if (norm_dist(rng) < inertialResistance) {
		isMoving = false;
	}
}

// ========= Ground Behavior =========
void MovableSolid::handleWhileGrounded(IMatrix& matrix, int x, int y) {
	if (!isMoving) return;
	// Blocked vertically: try to slide or move horizontally
	int dir = getRandomDirection(); // Randomly pick left or right
	// If falling fast enough, set horizontal velocity
	if (velocity_y >= 1.0f / absorption) {
		velocity_x = velocity_y * absorption * dir;
	}
	// Try to slide diagonally (sand piling)
	if (canReplaceElement(matrix, x + dir, y + 1)) {
		Element::swapElements(matrix, x, y, x + dir, y + 1);
		affectAdjacentNeighbors(matrix, x, y);
		velocity_x += 0.5f * dir;
	}
	else if (canReplaceElement(matrix, x - dir, y + 1)) {
		Element::swapElements(matrix, x, y, x - dir, y + 1);
		affectAdjacentNeighbors(matrix, x, y);
		velocity_x += 0.5f * dir;
	}
	// Try to move horizontally as far as possible using velocity_x
	int horiz_dir = (velocity_x > 0) ? 1 : (velocity_x < 0) ? -1 : 0;
	int max_steps = std::fabs(static_cast<int>(velocity_x));
	int last_valid_x = x;
	for (int i = 1; i <= max_steps; ++i) {
		int target_x = x + i * horiz_dir;
		if (canReplaceElement(matrix, target_x, y)) {
			last_valid_x = target_x;
		} else {
			break;
		}
	}
	if (last_valid_x != x) {
		Element::swapElements(matrix, x, y, last_valid_x, y);
		affectAdjacentNeighbors(matrix, x, y);
		velocity_x *= friction;
	}
}

// ========= Buoyancy Logic =========
void MovableSolid::handleBuoyancy(IMatrix& matrix, int x, int y) {
	Element* above = matrix.isInBounds(x, y - 1) ? matrix.getElement(x, y - 1) : nullptr;
	if (above && Element::isInstanceOf<Liquid>(above)) {
		velocity_y = 0.0f;
		Liquid* liquid = dynamic_cast<Liquid*>(above);
		if (!liquid) return;
		float diff = density - liquid->getDensity();
		if (diff < 0) {
			// This solid is less dense than the liquid above: try to rise up
			if (canReplaceElement(matrix, x, y - 1)) {
				if (norm_dist(rng) < std::fabs(diff)) {
					Element::swapElements(matrix, x, y, x, y - 1);
					return;
				}
			}
			// Small chance to drift left/right as it rises
			int horiz_dir = getRandomDirection();
			if (canReplaceElement(matrix, x + horiz_dir, y)) {
				if (norm_dist(rng) < std::fabs(diff)) {
					Element::swapElements(matrix, x, y, x + horiz_dir, y);
					return;
				}
			}
		} else if (diff > 0) {
			// This solid is more dense than the liquid above: try to sink (dense)
			if (canReplaceElement(matrix, x, y + 1)) {
				if (norm_dist(rng) < std::fabs(diff)) {
					Element::swapElements(matrix, x, y, x, y + 1);
					return;
				}
			}
		}
	}
}

// ========= Adjacent Effect =========
void MovableSolid::applyAdjacentNeighborEffect() {
	if (norm_dist(rng) > inertialResistance) {
		isMoving = true;
	}
}

// ========= Dissolution Behavior =========
bool MovableSolid::getShouldDissolve(IMatrix& matrix, int x, int y) {
	// If this solid has any dissolve chances defined
	if (dissolveChances.size() > 0) {
		for (const auto& pair : dissolveChances) {
			ElementType type = pair.first;
			float chance = pair.second;

			// Find all neighboring liquids of the specified type
			std::vector<Liquid*> liquids = getNeighborElementsOfType<Liquid>(matrix, type, x, y);
			std::vector<Liquid*> emptyLiquids;

			// The more matching neighbors, the higher the dissolve probability
			chance *= liquids.size();

			// Roll for dissolution: only proceed if random chance passes
			if (norm_dist(rng) < chance && matrix.isInBounds(x, y)) {
				// Collect up to dissolveThreshold unique empty liquids to dissolve into
				// Fix signed/unsigned comparison warnings by casting emptyLiquids.size() to int
				for (auto* liquid : liquids) {
					if (liquid->getDissolvedElement() == EMPTY) {
						auto it = std::find(emptyLiquids.begin(), emptyLiquids.end(), liquid);
						if (it == emptyLiquids.end()) {
							emptyLiquids.push_back(liquid);
							if (static_cast<int>(emptyLiquids.size()) >= dissolveThreshold) break;
						}
					}
				}

				// If enough empty liquids are available, perform the dissolve
				if (static_cast<int>(emptyLiquids.size()) >= dissolveThreshold) {
					// Mark the first liquid to destroy its dissolved element (if needed for cleanup)
					if (!emptyLiquids.empty()) {
						emptyLiquids.front()->setdestroyDissolvedElement(true);
					}
					// Set all selected liquids to contain the dissolved element type
					for (auto* liquid : emptyLiquids) {
						liquid->setDissolvedElement(getType());
					}
					return true;
				}
			}
		}
	}
	return false;
}

// ========= Replace Rule =========
bool MovableSolid::canReplaceElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	if (matrix.isEmpty(x, y)) return true;
	if (Element::isInstanceOf<Gas>(matrix.getElement(x, y))) return true;
	if (Element::isInstanceOf<Liquid>(matrix.getElement(x, y))) return true;
	return false;
}