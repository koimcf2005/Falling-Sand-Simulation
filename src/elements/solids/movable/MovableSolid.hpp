// -------------------------------------------
// MovableSolid.hpp
// -------------------------------------------
// Base class for all movable solid elements (e.g., sand, powder).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef MOVABLE_SOLID_HPP
#define MOVABLE_SOLID_HPP

#include <unordered_map>
#include "Solid.hpp"

/**
 * @brief Base class for all movable solid elements (e.g., sand, powder).
 * 
 * Provides velocity and movement accumulation for smooth simulation.
 * Does not implement update logic directly; intended for inheritance.
 */
class MovableSolid : public Solid {
public:
	/// ==================== Construction/Destruction ====================
	MovableSolid(ElementType type, int x, int y) : Solid(type, x, y) {}

	/// ==================== Core Interface ====================
	void update(IMatrix& matrix, int x, int y) override;

	void setMovingByInertia();

protected:
	/// ==================== Utilities ====================
	bool canReplaceElement(IMatrix& matrix, int x, int y) const;
	bool handleDissolving(IMatrix& matrix, int x, int y);
	void handleDensity(IMatrix& matrix, int x, int y);
	void affectAdjacentNeighbors(IMatrix& matrix, int x, int y);

	/// ==================== Physical Properties ====================
	const float GRAVITY = 0.1f;      ///< Gravitational constant affecting the solid
	float velocity_y = 1.0f;         ///< Vertical velocity
	float velocity_x = 0.0f;         ///< Horizontal velocity
	float accumulated_y = 0.0f;      ///< Accumulated vertical movement
	float accumulated_x = 0.0f;      ///< Accumulated horizontal movement
	float friction = 0.5f;           ///< Friction
	float absorption = 0.5f;         ///< Absorption
	float inertialResistance = 0.5f; ///< Resistance to inertia
};

#endif // MOVABLE_SOLID_HPP