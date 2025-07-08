// src/elements/PhysicsParticle.cpp
#include "src/elements/PhysicsParticle.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/elements/gases/Gas.hpp"
#include "src/elements/liquids/Liquid.hpp"

PhysicsParticle::PhysicsParticle(ElementType representedType, int x, int y, float velX, float velY)
    : Element(PHYSICS_PARTICLE, x, y),
      representedType(representedType),
      velocity_x(velX),
      velocity_y(velY),
      accumulated_x(0.0f)
{
    // Take on the color of the represented element
    color = ElementFactory::getColorByElementType(representedType, x, y);
    backup_color = color;
    
    // Set density based on represented element (approximate)
    Element* tempElement = ElementFactory::createElementFromType(representedType, x, y);
    if (tempElement) {
        density = tempElement->getDensity();
        delete tempElement;
    }
}

void PhysicsParticle::update(IMatrix& matrix, int x, int y) {
    if (checkIfUpdated()) return;
    
    movedThisFrame = false;
    
    // Apply air resistance
    velocity_x *= AIR_RESISTANCE;
    velocity_y *= AIR_RESISTANCE;
    
    // Apply gravity
    velocity_y += GRAVITY;
    
    // Clamp velocities to reasonable limits
    velocity_x = std::clamp(velocity_x, -10.0f, 10.0f);
    velocity_y = std::clamp(velocity_y, -10.0f, 10.0f);
    
    // Update accumulated positions
    accumulated_x += velocity_x;
    accumulated_y += velocity_y;
    
    // Calculate movement steps
    int move_x = static_cast<int>(accumulated_x);
    int move_y = static_cast<int>(accumulated_y);
    
    // Handle horizontal movement
    if (move_x != 0) {
        int target_x = x + move_x;
        if (canReplaceElement(matrix, target_x, y)) {
            swapElements(matrix, x, y, target_x, y);
            accumulated_x -= move_x;
            movedThisFrame = true;
            x = target_x; // Update position for vertical movement
        } else {
            // Bounce off horizontal obstacle
            velocity_x = -velocity_x * BOUNCE_DAMPING;
            accumulated_x = 0.0f;
        }
    }
    
    // Handle vertical movement
    if (move_y != 0) {
        int target_y = y + move_y;
        if (canReplaceElement(matrix, x, target_y)) {
            swapElements(matrix, x, y, x, target_y);
            accumulated_y -= move_y;
            movedThisFrame = true;
        } else {
            // Bounce off vertical obstacle
            velocity_y = -velocity_y * BOUNCE_DAMPING;
            accumulated_y = 0.0f;
        }
    }
    
    // Check if particle should revert to original element
    if (isEffectivelyStationary()) {
        revertToOriginalElement(matrix, x, y);
        return;
    }
    
    isMoving = movedThisFrame;
}

void PhysicsParticle::setVelocity(float velX, float velY) {
    velocity_x = velX;
    velocity_y = velY;
}

void PhysicsParticle::setRepresentedType(ElementType type) {
    representedType = type;
    color = ElementFactory::getColorByElementType(type, 0, 0);
    backup_color = color;
}

ElementType PhysicsParticle::getRepresentedType() const {
    return representedType;
}

float PhysicsParticle::getVelocityX() const {
    return velocity_x;
}

float PhysicsParticle::getVelocityY() const {
    return velocity_y;
}

bool PhysicsParticle::canReplaceElement(IMatrix& matrix, int x, int y) const {
    if (!matrix.isInBounds(x, y)) return false;
    if (matrix.isEmpty(x, y)) return true;
    if (Element::isInstanceOf<Gas>(matrix.getElement(x, y))) return true;
    if (Element::isInstanceOf<Liquid>(matrix.getElement(x, y))) return true;
    return false;
}

void PhysicsParticle::revertToOriginalElement(IMatrix& matrix, int x, int y) {
    Element* originalElement = ElementFactory::createElementFromType(representedType, x, y);
    if (originalElement) {
        Element* currentElement = matrix.getElement(x, y);
        matrix.getElement(x, y) = originalElement;
        delete currentElement;
    }
}

bool PhysicsParticle::isEffectivelyStationary() const {
    return std::abs(velocity_x) < MIN_VELOCITY_THRESHOLD && 
           std::abs(velocity_y) < MIN_VELOCITY_THRESHOLD &&
           !movedThisFrame;
}