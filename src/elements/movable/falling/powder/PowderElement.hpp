#ifndef POWDER_ELEMENT_HPP
#define POWDER_ELEMENT_HPP

#include "src/elements/movable/falling/FallingElement.hpp"

/**
 * @brief Represents a powder element that falls and interacts with friction and impact.
 * 
 * Inherits from FallingElement and adds properties for friction, impact absorption,
 * and inertial resistance.
 */
class PowderElement : public FallingElement {
public:
	/**
	 * @brief Construct a new PowderElement.
	 * @param type The element type.
	 * @param x Initial x-coordinate.
	 * @param y Initial y-coordinate.
	 */
	PowderElement(ElementType type, int x, int y) : FallingElement(type, x, y) {}

	/**
	 * @brief Update the powder element for the simulation step.
	 * @param matrix The simulation matrix.
	 */
	void update(IMatrix& matrix) override;
protected:
	/**
	 * @brief Determine if this powder can swap with the element at (x, y).
	 * @param matrix The simulation matrix.
	 * @param x The x-coordinate to check.
	 * @param y The y-coordinate to check.
	 * @return True if swap is allowed, false otherwise.
	 */
	bool canSwapWithElement(IMatrix& matrix, int x, int y) const override;

	/**
	 * @brief Respond to the movement of a neighboring element.
	 */
	void recieveNeighborEffect() override;

	/**
	 * @brief Handle logic when the powder is grounded.
	 * @param matrix The simulation matrix.
	 */
	void handleGrounded(IMatrix& matrix) override;

	void handleBuoyancy(IMatrix&) override;

	/**
	 * @brief Friction coefficient for powder movement.
	 */
	float m_Friction = 0.1f;

	/**
	 * @brief Impact absorption factor for collisions.
	 */
	float m_ImpactAbsorption = 0.5f;

	/**
	 * @brief Inertial resistance to movement.
	 */
	float m_InertialResistance = 0.0f;
};

#endif // POWDER_ELEMENT_HPP
