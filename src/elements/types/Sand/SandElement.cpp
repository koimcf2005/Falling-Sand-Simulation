#include "src/elements/types/Sand/SandElement.hpp"
#include "src/elements/ElementUtilities.hpp"
#include "src/core/Utilities.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Components.hpp"
#include "src/matrix/Matrix.hpp"

void Sand::initialize(entt::entity entity) {
	ComponentManager::addComponent<Velocity>(entity, Velocity(0, 0));
	// ComponentManager::addComponent<Health>(entity, Health(100));
	// ComponentManager::addComponent<Temperature>(entity, Temperature(100.0f));
}

void Sand::update(Matrix& matrix, entt::entity entity) {
	Position& position = ComponentManager::getComponent<Position>(entity);
	Velocity& velocity = ComponentManager::getComponent<Velocity>(entity);
	// Health& health = ComponentManager::getComponent<Health>(entity);
	// Temperature& temperature = ComponentManager::getComponent<Temperature>(entity);

	static const float GRAVITY = 0.01f;

	if (ElementUtilities::canSwapWithElement(matrix, position.x, position.y + 1)) {
		velocity.vy += GRAVITY;
		velocity.ay += velocity.vy;
		int delta_y = std::clamp(static_cast<int>(velocity.ay), -32, 32);

		if (delta_y != 0) {
			int last_valid_y = position.y;

			for (int i = 1; i <= std::abs(delta_y); ++i) {
				int check_y = position.y + (delta_y > 0 ? i : -i);
				if (ElementUtilities::canSwapWithElement(matrix, position.x, check_y)) {
					last_valid_y = check_y;
				}
				else {
					break;
				}
			}
			if (last_valid_y != position.y) {
				if (matrix.entityHasComponent<Velocity>(position.x, position.y - 1)) {
					Velocity& above_vel = matrix.getEntityComponent<Velocity>(position.x, position.y - 1);
					above_vel.vy = velocity.vy; 
				}
				matrix.swapEntities(position.x, position.y, position.x, last_valid_y);
				velocity.ay -= (last_valid_y - position.y);
			}
		}
	}
	else {
		velocity.vy = 0.0f;
		velocity.ay = 0.0f;
		int dir = RNG::getRandomDirection();
		if (ElementUtilities::canSwapWithElement(matrix, position.x + dir, position.y + 1)) {
			matrix.swapEntities(position.x, position.y, position.x + dir, position.y + 1);
		}
		else if (ElementUtilities::canSwapWithElement(matrix, position.x - dir, position.y + 1)) {
			matrix.swapEntities(position.x, position.y, position.x - dir, position.y + 1);
		}
	}
	if (velocity.vy > 0) matrix.activateChunk(position.x, position.y);
}