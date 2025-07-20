#ifndef RISING_ELEMENT_HPP
#define RISING_ELEMENT_HPP

#include "src/elements/movable/MovableElement.hpp"

/**
 * @brief Abstract base class for elements that rise due to negative gravity (buoyancy).
 *
 * Inherits from MovableElement and provides basic rising logic.
 * Derived classes must implement the `handleCeilinged` method to define behavior
 * when the element is no longer rising.
 */
class RisingElement : public MovableElement {
protected:
	// ========== Construction ==========
	RisingElement(ElementType type, int x, int y) : MovableElement(type, x, y) {}

	/**
	 * @brief Determine whether this element can swap with another at the given coordinates.
	 *
	 * Must be overridden in derived classes to define specific rules.
	 */
	bool canSwapWithElement(IMatrix& matrix, int x, int y) const override = 0;

	/**
	 * @brief React to the movement of a neighboring element.
	 *
	 * Default implementation does nothing. Can be overridden in subclasses.
	 */
	void recieveNeighborEffect() override = 0;

	/**
	 * @brief Handles the logic for rising movement (negative gravity).
	 *
	 * Applies negative gravity, velocity, and collision detection, and updates position accordingly.
	 *
	 * @param matrix The simulation matrix.
	 */
	void handleRising(IMatrix& matrix);

	/**
	 * @brief Called when the element is considered "ceilinged" (can't rise further).
	 *
	 * Subclasses must define how ceilinged elements behave.
	 *
	 * @param matrix The simulation matrix.
	 */
	virtual void handleCeilinged(IMatrix& matrix) = 0;

	void handleBuoyancy(IMatrix& matrix) override = 0;

	float m_ChanceOfHorizontal = 0.5f;

};

#endif // RISING_ELEMENT_HPP
