#ifndef GAS_ELEMENT_HPP
#define GAS_ELEMENT_HPP

#include "src/elements/movable/rising/RisingElement.hpp"

/**
 * @brief Represents a gas element that rises due to buoyancy and interacts by density.
 * 
 * Inherits from RisingElement and BuoyantElement.
 */
class GasElement : public RisingElement {
public:
	/**
	 * @brief Construct a new GasElement.
	 * @param type The element type.
	 * @param x Initial x-coordinate.
	 * @param y Initial y-coordinate.
	 */
	GasElement(ElementType type, int x, int y) : RisingElement(type, x, y) {}

	/**
	 * @brief Update the gas element for the simulation step.
	 * @param matrix The simulation matrix.
	 */
	void update(IMatrix& matrix) override;
protected:
	/**
	 * @brief Determine if this gas can swap with the element at (x, y).
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
	 * @brief Handle logic when the gas is "ceilinged" (can't rise further).
	 * @param matrix The simulation matrix.
	 */
	void handleCeilinged(IMatrix& matrix) override;

	void handleBuoyancy(IMatrix&) override { return; }

	int m_TimeUntilDeath = 100;
	float m_ChanceOfDeathAfterTimer = 0.01f;
};

#endif // GAS_ELEMENT_HPP
