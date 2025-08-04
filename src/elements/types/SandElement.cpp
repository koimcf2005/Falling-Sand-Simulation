// SandElement.cpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// *Explain this file*
//
// Implements the Sand element's initialization and update functions for the
// simulation. Used to define Sand-specific behavior.

#include "falling-sand-sim/elements/types/SandElement.hpp"

#include "falling-sand-sim/components/EnTTManager.hpp"
#include "falling-sand-sim/components/Components.hpp"
#include "falling-sand-sim/elements/ElementUtilities.hpp"
#include "falling-sand-sim/utils/Utilities.hpp"
#include "falling-sand-sim/world/Matrix.hpp"

#include <iostream>

void Sand::initialize(entt::entity entity) {
	// EnTTManager::addComponent<Velocity>(entity, Velocity(0, 1.0f));
	// EnTTManager::addComponent<MovementState>(entity, MovementState(true, false, false));
}

void Sand::update(Matrix& matrix, entt::entity entity, const int x, const int y) {
	if (ElementUtilities::canSwapWithElement(matrix, x, y + 1)) {
		matrix.swapEntities(x, y, x, y + 1);
		return;
	}
	int dir = RNG::getRandomDirection();
	if (ElementUtilities::canSwapWithElement(matrix, x + dir, y + 1) && ElementUtilities::canSwapWithElement(matrix, x + dir, y)) {
		matrix.swapEntities(x, y, x + dir, y + 1);
	} else if (ElementUtilities::canSwapWithElement(matrix, x - dir, y + 1) && ElementUtilities::canSwapWithElement(matrix, x - dir, y)) {
		matrix.swapEntities(x, y, x - dir, y + 1);
	}
	// Velocity& velocity = EnTTManager::getComponent<Velocity>(entity);
	// MovementState& movementState = EnTTManager::getComponent<MovementState>(entity);

	// static const float GRAVITY = 0.2f;

	// // Check if the sand can move downward
	// if (ElementUtilities::canSwapWithElement(matrix, x, y + 1)) {
	// 	movementState.setMoving(true);

	// 	// Apply gravity to velocity
	// 	velocity.vy += GRAVITY;
	// 	velocity.ay += velocity.vy;
	// 	int delta_y = std::clamp(static_cast<int>(velocity.ay), -32, 32);

	// 	if (delta_y != 0) {
	// 		int last_valid_y = y;

	// 		// Find the furthest valid position downward
	// 		for (int i = 1; i <= std::abs(delta_y); ++i) {
	// 			int check_y = y + (delta_y > 0 ? i : -i);
	// 			if (ElementUtilities::canSwapWithElement(matrix, x, check_y)) {
	// 				last_valid_y = check_y;
	// 			} else {
	// 				break;
	// 			}
	// 		}

	// 		// If the sand moved, update its position and state
	// 		if (last_valid_y != y) {
	// 			// Update the movement state and velocity of the entity above
	// 			auto entity_above = matrix.getEntity(x, y - 1);
	// 			if (entity_above == entt::null) goto swap;
	// 			if (auto velocity_above = EnTTManager::getComponentIfExists<Velocity>(entity)) {
	// 				velocity_above->vy = velocity.vy;
	// 			}
	// 			swap:
	// 			matrix.swapEntities(x, y, x, last_valid_y);
	// 			velocity.ay -= (last_valid_y - y);
	// 		}
	// 	}
	// } else {
	// 	// If the sand cannot move downward, reset its velocity
	// 	velocity.vy = 0.0f;
	// 	velocity.ay = 0.0f;

	// 	// If the sand is not moving, exit early
	// 	if (!movementState.isMoving()) {
	// 		return;
	// 	}

	// 	// Attempt to move diagonally
	// 	int dir = RNG::getRandomDirection();
	// 	if (ElementUtilities::canSwapWithElement(matrix, x + dir, y + 1) && ElementUtilities::canSwapWithElement(matrix, x + dir, y)) {
	// 		matrix.swapEntities(x, y, x + dir, y + 1);
	// 	} else if (ElementUtilities::canSwapWithElement(matrix, x - dir, y + 1) && ElementUtilities::canSwapWithElement(matrix, x - dir, y)) {
	// 		matrix.swapEntities(x, y, x - dir, y + 1);
	// 	}
	// }
}