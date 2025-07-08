// src/elements/PhysicsParticle.hpp
#ifndef PHYSICS_PARTICLE_HPP
#define PHYSICS_PARTICLE_HPP

#include "src/elements/Element.hpp"

/**
 * @brief A temporary physics-based particle that handles movement with velocity.
 * 
 * PhysicsParticle is used when elements need complex movement with x/y velocity.
 * It stores the original element type and reverts back when it stops moving.
 */
class PhysicsParticle : public Element {
public:
    PhysicsParticle(ElementType representedType, int x, int y, float velX = 0.0f, float velY = 0.0f);
    
    void update(IMatrix& matrix, int x, int y) override;
    
    // Setters for physics properties
    void setVelocity(float velX, float velY);
    void setRepresentedType(ElementType type);
    
    // Getters
    ElementType getRepresentedType() const;
    float getVelocityX() const;
    float getVelocityY() const;

protected:
    bool canReplaceElement(IMatrix& matrix, int x, int y) const override;

private:
    ElementType representedType;
    float velocity_x;
    float velocity_y;
    float accumulated_x;
    
    static constexpr float BOUNCE_DAMPING = 0.7f;
    static constexpr float MIN_VELOCITY_THRESHOLD = 0.1f;
    static constexpr float AIR_RESISTANCE = 0.99f;
    
    void revertToOriginalElement(IMatrix& matrix, int x, int y);
    bool isEffectivelyStationary() const;
};

#endif // PHYSICS_PARTICLE_HPP