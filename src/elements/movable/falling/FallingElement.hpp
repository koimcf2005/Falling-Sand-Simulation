#ifndef FALLING_ELEMENT_HPP
#define FALLING_ELEMENT_HPP

#include "src/elements/movable/MovableElement.hpp"

/**
 * @brief Abstract base class for elements that fall due to gravity.
 *
 * Inherits from MovableElement and provides basic gravity and falling logic.
 * Derived classes must implement the `handleGrounded` method to define behavior
 * when the element is no longer falling.
 */
class FallingElement : public MovableElement {
protected:
	// ========== Construction ==========
	FallingElement(ElementType type, int x, int y) : MovableElement(type, x, y) {}

	/**
	 * @brief Determine whether this element can swap with another at the given coordinates.
	 *
	 * This function is used to decide if the element can move to the specified (x, y) location.
	 * Must be overridden in derived classes to define specific rules.
	 *
	 * @param matrix The simulation matrix.
	 * @param x The x-coordinate to test.
	 * @param y The y-coordinate to test.
	 * @return true if a swap is allowed; false otherwise.
	 */
	bool canSwapWithElement(IMatrix& matrix, int x, int y) const override = 0;

	/**
	 * @brief React to the movement of a neighboring element.
	 *
	 * Default implementation does nothing. Can be overridden in subclasses.
	 */
	void recieveNeighborEffect() override = 0;

	/**
	 * @brief Handles the logic for gravity-based falling movement.
	 *
	 * Applies gravity, velocity, and collision detection, and updates position accordingly.
	 *
	 * @param matrix The simulation matrix.
	 */
	void handleFalling(IMatrix& matrix);

	/**
	 * @brief Called when the element is considered grounded (not falling).
	 *
	 * Subclasses must define how grounded elements behave.
	 *
	 * @param matrix The simulation matrix.
	 */
	virtual void handleGrounded(IMatrix& matrix) = 0;

	void handleBuoyancy(IMatrix& matrix) override = 0;

	/// Constant gravitational acceleration per frame
	const float GRAVITY = 0.2;
};

#endif // FALLING_ELEMENT_HPP
