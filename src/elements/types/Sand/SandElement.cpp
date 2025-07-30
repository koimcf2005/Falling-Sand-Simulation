#include "src/elements/types/Sand/SandElement.hpp"
#include "src/elements/ElementUtilities.hpp"
#include "src/core/Utilities.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Components.hpp"
#include "src/matrix/Matrix.hpp"
#include <iostream>

void Sand::initialize(entt::entity entity) {
	ComponentManager::addComponent<Velocity>(entity, Velocity(0, 1.0f));
	ComponentManager::addComponent<MovementState>(entity, MovementState(true, false, false));
}

void Sand::update(Matrix& matrix, entt::entity entity, const int x, const int y) {
	Velocity& velocity = ComponentManager::getComponent<Velocity>(entity);
	MovementState& movementState = ComponentManager::getComponent<MovementState>(entity);

	static const float GRAVITY = 0.2f;

	// Check if the sand can move downward
	if (ElementUtilities::canSwapWithElement(matrix, x, y + 1)) {
		movementState.setMoving(true);

		// Apply gravity to velocity
		velocity.vy += GRAVITY;
		velocity.ay += velocity.vy;
		int delta_y = std::clamp(static_cast<int>(velocity.ay), -32, 32);

		if (delta_y != 0) {
			int last_valid_y = y;

			// Find the furthest valid position downward
			for (int i = 1; i <= std::abs(delta_y); ++i) {
				int check_y = y + (delta_y > 0 ? i : -i);
				if (ElementUtilities::canSwapWithElement(matrix, x, check_y)) {
					last_valid_y = check_y;
				} else {
					break;
				}
			}

			// If the sand moved, update its position and state
			if (last_valid_y != y) {
				// Update the movement state and velocity of the entity above
				auto entity_above = matrix.getEntity(x, y - 1);
				if (entity_above == entt::null) goto swap;
				if (auto velocity_above = ComponentManager::getComponentIfExists<Velocity>(entity)) {
					velocity_above->vy = velocity.vy;
				}
				swap:
				matrix.swapEntities(x, y, x, last_valid_y);
				velocity.ay -= (last_valid_y - y);
			}
		}
	} else {
		// If the sand cannot move downward, reset its velocity
		velocity.vy = 0.0f;
		velocity.ay = 0.0f;

		// If the sand is not moving, exit early
		if (!movementState.isMoving()) {
			return;
		}

		// Attempt to move diagonally
		int dir = RNG::getRandomDirection();
		if (ElementUtilities::canSwapWithElement(matrix, x + dir, y + 1)) {
			matrix.swapEntities(x, y, x + dir, y + 1);
		} else if (ElementUtilities::canSwapWithElement(matrix, x - dir, y + 1)) {
			matrix.swapEntities(x, y, x - dir, y + 1);
		}
	}
}