// -------------------------------------------
// Gas.hpp
// -------------------------------------------
// Base class for all gas elements (e.g., steam, smoke).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef GAS_HPP
#define GAS_HPP

#include "src/elements/Element.hpp"

/**
 * @brief Base class for all gas elements (e.g., steam, smoke).
 * 
 * Provides velocity and movement accumulation for smooth simulation.
 * Does not implement update logic directly; intended for inheritance.
 */
class Gas : public Element {
public:
	/// ==================== Construction/Destruction ====================
	Gas(ElementType type, int x, int y) : Element(type, x, y) {}

	/// ==================== Core Interface ====================
	/**
	 * @brief Update function to be implemented by derived classes.
	 */
	void update(IMatrix& matrix, int x, int y) override;

protected:
	/// ==================== Physical Properties ====================
	const float UPWARD_MOMENTUM = 0.1f; ///< Upward force for gases.
	float velocity_x = 0.0f;            ///< Horizontal velocity.
	float velocity_y = -1.0f;           ///< Vertical velocity (upwards).
	float accumulated_x = 0.0f;         ///< Accumulated horizontal movement.
	float accumulated_y = 0.0f;         ///< Accumulated vertical movement.
	int stoppedMovingCount = 0;         ///< Counter for stopped movement.
	int stoppedMovingThreshold = 5;     ///< Threshold for stopped state.
	bool isMoving = true;               ///< Is the gas currently moving.

	float chanceOfHorizontal = 0.75f;   ///< Chance to move horizontally.
	float chanceOfDeath = 0.01f;        ///< Chance to disappear.
};

#endif // GAS_HPP