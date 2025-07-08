// src/elements/solids/movable/MovableSolid.hpp
#ifndef MOVABLE_SOLID_HPP
#define MOVABLE_SOLID_HPP

#include <unordered_map>
#include "src/elements/solids/Solid.hpp"

/**
 * @brief Base class for all movable solid elements (e.g., sand, powder).
 * 
 * Provides horizontal velocity and advanced movement simulation.
 * Does not implement main update logic; designed to be extended.
 */
class MovableSolid : public Solid {
public:
	// ========= Constructor / Destructor =========
	MovableSolid(ElementType type, int x, int y) : Solid(type, x, y) {}
	virtual ~MovableSolid() = default;

	// ========= Core Interface =========
	void update(IMatrix& matrix, int x, int y) override;

	// ========= Movement / Behavior =========
	bool canReplaceElement(IMatrix& matrix, int x, int y) const override;
	void handleWhileGrounded(IMatrix& matrix, int x, int y) override;

protected:
	// ========= Physical Response =========
	void handleBuoyancy(IMatrix& matrix, int x, int y) override;
	void applyAdjacentNeighborEffect() override;

	// ========= Special Interaction =========
	bool getShouldDissolve(IMatrix& matrix, int x, int y);

	// ========= Horizontal Motion Properties =========
	float velocity_x = 0.0f;        ///< Horizontal velocity
	float accumulated_x = 0.0f;     ///< Accumulated horizontal movement
};

#endif // MOVABLE_SOLID_HPP
