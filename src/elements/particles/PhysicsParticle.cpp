// src/elements/PhysicsParticle.cpp
#include "src/elements/particles/PhysicsParticle.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/elements/gases/Gas.hpp"
#include "src/elements/liquids/Liquid.hpp"

// ========= Constructor =========
PhysicsParticle::PhysicsParticle(ElementType representedType, SDL_Color elemColor, int x, int y, float velX, float velY)
	: Element(PHYSICS_PARTICLE, x, y),
	  representedType(representedType),
	  velocity_x(velX),
	  velocity_y(velY),
	  accumulated_x(0.0f)
{
	// Take on the color of the represented element
	color = elemColor;
	backup_color = color;

	// Set density based on represented element (approximate)
	Element* tempElement = ElementFactory::createElementFromType(representedType, x, y);
	if (tempElement) {
		density = tempElement->getDensity();
		delete tempElement;
	}
}

// ========= Update Loop =========
void PhysicsParticle::update(IMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;

	movedThisFrame = false;

	// Apply air resistance
	velocity_x *= AIR_RESISTANCE;
	velocity_y *= AIR_RESISTANCE;

	// Apply gravity
	velocity_y += GRAVITY;

	// Clamp velocities
	velocity_x = std::clamp(velocity_x, -10.0f, 10.0f);
	velocity_y = std::clamp(velocity_y, -10.0f, 10.0f);

	// Accumulate movement
	accumulated_x += velocity_x;
	accumulated_y += velocity_y;

	// Determine movement
	int move_x = static_cast<int>(accumulated_x);
	int move_y = static_cast<int>(accumulated_y);

	// Horizontal movement
	if (move_x != 0) {
		int target_x = x + move_x;
		if (canReplaceElement(matrix, target_x, y)) {
			swapElements(matrix, x, y, target_x, y);
			accumulated_x -= move_x;
			movedThisFrame = true;
			x = target_x; // Update x position for vertical
		} else {
			velocity_x = -velocity_x * BOUNCE_DAMPING;
			accumulated_x = 0.0f;
		}
	}

	// Vertical movement
	if (move_y != 0) {
		int target_y = y + move_y;
		if (canReplaceElement(matrix, x, target_y)) {
			swapElements(matrix, x, y, x, target_y);
			accumulated_y -= move_y;
			movedThisFrame = true;
		} else {
			velocity_y = -velocity_y * BOUNCE_DAMPING;
			accumulated_y = 0.0f;
		}
	}

	// Check for reversion
	if (isEffectivelyStationary()) {
		revertToOriginalElement(matrix, x, y);
		return;
	}

	isMoving = movedThisFrame;
}

// ========= Setters =========
void PhysicsParticle::setVelocity(float velX, float velY) {
	velocity_x = velX;
	velocity_y = velY;
}

void PhysicsParticle::setRepresentedType(ElementType type) {
	representedType = type;
	color = ElementFactory::getColorByElementType(type, 0, 0);
	backup_color = color;
}

// ========= Getters =========
ElementType PhysicsParticle::getRepresentedType() const {
	return representedType;
}

float PhysicsParticle::getVelocityX() const {
	return velocity_x;
}

float PhysicsParticle::getVelocityY() const {
	return velocity_y;
}

// ========= Replace Rule =========
bool PhysicsParticle::canReplaceElement(IMatrix& matrix, int x, int y) const {
	if (!matrix.isInBounds(x, y)) return false;
	if (matrix.isEmpty(x, y)) return true;
	if (Element::isInstanceOf<Gas>(matrix.getElement(x, y))) return true;
	if (Element::isInstanceOf<Liquid>(matrix.getElement(x, y))) return true;
	return false;
}

// ========= Element Reversion =========
void PhysicsParticle::revertToOriginalElement(IMatrix& matrix, int x, int y) {
	Element* originalElement = ElementFactory::createElementFromType(representedType, x, y);
	if (originalElement) {
		originalElement->setColor(color);
		Element* currentElement = matrix.getElement(x, y);
		matrix.getElement(x, y) = originalElement;
		delete currentElement;
	}
}

// ========= Stationary Check =========
bool PhysicsParticle::isEffectivelyStationary() const {
	return std::abs(velocity_x) < MIN_VELOCITY_THRESHOLD &&
			std::abs(velocity_y) < MIN_VELOCITY_THRESHOLD &&
			!movedThisFrame;
}

// ========= Conversion Utility =========
void PhysicsParticle::convertElementToPhysicsParticle(IMatrix& matrix, int x, int y, float velX, float velY) {
	Element* element = matrix.getElement(x, y);
	PhysicsParticle* particle = new PhysicsParticle(
		element->getType(),
		element->getColor(),
		x, y,
		velX,
		velY
	);
	Element* currentElement = matrix.getElement(x, y);
	matrix.getElement(x, y) = particle;
	delete currentElement;
}
