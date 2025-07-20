#ifndef MOVABLE_ELEMENT_HPP
#define MOVABLE_ELEMENT_HPP

#include "src/elements/Element.hpp"

/**
 * @brief Base class for elements that can move within the simulation grid.
 * 
 * This class adds velocity, movement state, and swap logic to enable physical
 * interactions such as falling, sliding, and flowing. Elements that inherit
 * from MovableElement will be able to change position over time.
 */
class MovableElement : public Element {
public:
	// ========== Construction ==========
	MovableElement(ElementType type, int x, int y) : Element(type, x, y) {}

	/**
	 * @brief Set horizontal velocity.
	 * 
	 * @param velocityX The horizontal velocity component.
	 */
	void setVelocityX(float velocityX);

	/**
	 * @brief Set vertical velocity.
	 * 
	 * @param velocityY The vertical velocity component.
	 */
	void setVelocityY(float velocityY);

	/**
	 * @brief Add to the current horizontal velocity.
	 * 
	 * @param velocityX Amount to add to horizontal velocity.
	 */
	void addVelocityX(float velocityX);

	/**
	 * @brief Add to the current vertical velocity.
	 * 
	 * @param velocityY Amount to add to vertical velocity.
	 */
	void addVelocityY(float velocityY);

	/**
	 * @return Current horizontal velocity.
	 */
	float getVelocityX() const;

	/**
	 * @return Current vertical velocity.
	 */
	float getVelocityY() const;

	float getDensity() const;

	/**
	 * @return Whether the element is currently marked as moving.
	 */
	bool getIsMoving() const;

	/**
	 * @brief Set whether the element is currently marked as moving.
	 * 
	 * @param isMoving True if the element is actively moving.
	 */
	void setIsMoving(bool isMoving);

	// Movement state management
	void setMovedThisFrame(bool moved) { m_MovedThisFrame = moved; }
	bool getMovedThisFrame() const { return m_MovedThisFrame; }

	void setWasMoving(bool wasMoving) { m_WasMoving = wasMoving; }
	bool getWasMoving() const { return m_WasMoving; }

protected:
	/**
	 * @brief Swap this element with another at the specified coordinates.
	 * 
	 * Updates both elements' positions and movement states.
	 * 
	 * @param matrix The simulation matrix.
	 * @param x Target x-coordinate.
	 * @param y Target y-coordinate.
	 */
	void swapWithElement(IMatrix& matrix, int x, int y);

	/**
	 * @brief Swap two elements at arbitrary coordinates.
	 * 
	 * Updates both elements' positions and movement states.
	 * 
	 * @param matrix The simulation matrix.
	 * @param x1 First element's x-coordinate.
	 * @param y1 First element's y-coordinate.
	 * @param x2 Second element's x-coordinate.
	 * @param y2 Second element's y-coordinate.
	 */
	void swapElements(IMatrix& matrix, int x1, int y1, int x2, int y2);

	/**
	 * @brief Determine whether this element can swap with another at the given coordinates.
	 * 
	 * This is a pure virtual function that must be implemented by derived classes.
	 * 
	 * @param matrix The simulation matrix.
	 * @param x Target x-coordinate.
	 * @param y Target y-coordinate.
	 * @return true if a swap is permitted; false otherwise.
	 */
	virtual bool canSwapWithElement(IMatrix& matrix, int x, int y) const = 0;

	/**
	 * @brief Notifies adjacent elements that this one has moved.
	 * 
	 * Used to trigger responses in neighboring elements, such as causing them to fall.
	 */
	void affectAdjacentNeighbors(IMatrix& matrix);

	/**
	 * @brief Respond to the movement of a neighboring element.
	 * 
	 * This is a pure virtual function that derived classes must implement to define their response.
	 */
	virtual void recieveNeighborEffect() = 0;

	virtual void handleBuoyancy(IMatrix& matrix) = 0;

	float m_Density = 0.5f;

	// ========== Movement State Variables ==========

	float m_VelocityX = 0;       ///< Horizontal velocity component
	float m_VelocityY = 0.0f;       ///< Vertical velocity component

	float m_AccumulatedX = 0;    ///< Accumulator for subpixel movement (horizontal)
	float m_AccumulatedY = 0;    ///< Accumulator for subpixel movement (vertical)

	bool m_MovedThisFrame = false; ///< Whether the element moved during this update
	bool m_WasMoving = false;      ///< Whether the element was moving in the last frame
	bool m_IsMoving = false;       ///< Whether the element is currently considered moving

	/**
	 * @brief Update movement flags for this frame.
	 * 
	 * Should be called once per frame at the start of update().
	 * Sets mWasMoving to previous mIsMoving, resets mMovedThisFrame to false.
	 */
	void handleMovementFlags() {
		m_WasMoving = m_IsMoving;
		m_MovedThisFrame = false;
	}


};

#endif // MOVABLE_ELEMENT_HPP
