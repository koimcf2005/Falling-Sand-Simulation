// src/elements/PhysicsParticle.hpp
#ifndef PHYSICS_PARTICLE_HPP
#define PHYSICS_PARTICLE_HPP

#include "src/elements/Element.hpp"

/**
 * @brief A temporary physics-based particle that handles movement with velocity.
 *
 * PhysicsParticle is used for elements that require complex movement, such as x/y velocity.
 * It stores the original element type and reverts back once movement has stopped.
 */
class PhysicsParticle : public Element {
public:
	// ========= Constructor =========
	PhysicsParticle(ElementType representedType, SDL_Color elemColor, int x, int y, float velX = 0.0f, float velY = 0.0f);

	// ========= Core Interface =========
	void update(IMatrix& matrix, int x, int y) override;

	// ========= Setters =========
	void setVelocity(float velX, float velY);
	void setRepresentedType(ElementType type);

	// ========= Getters =========
	ElementType getRepresentedType() const;
	float getVelocityX() const;
	float getVelocityY() const;

	// ========= Utility =========
	static void convertElementToPhysicsParticle(IMatrix& matrix, int x, int y, float velX = 0.0f, float velY = 0.0f);

protected:
	// ========= Movement / Behavior =========
	bool canReplaceElement(IMatrix& matrix, int x, int y) const override;

private:
	// ========= Physics State =========
	ElementType representedType;    ///< The element type being temporarily simulated
	float velocity_x;               ///< Current x-axis velocity
	float velocity_y;               ///< Current y-axis velocity
	float accumulated_x;            ///< Accumulated x movement

	// ========= Constants =========
	static constexpr float BOUNCE_DAMPING = 0.7f;             ///< Energy retained after bouncing
	static constexpr float MIN_VELOCITY_THRESHOLD = 0.1f;     ///< Minimum velocity before reverting
	static constexpr float AIR_RESISTANCE = 0.99f;            ///< Resistance applied each frame

	// ========= Internal Helpers =========
	void revertToOriginalElement(IMatrix& matrix, int x, int y);
	bool isEffectivelyStationary() const;
};

#endif // PHYSICS_PARTICLE_HPP
