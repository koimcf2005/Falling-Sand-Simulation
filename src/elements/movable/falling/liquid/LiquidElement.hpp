#ifndef LIQUID_ELEMENT_HPP
#define LIQUID_ELEMENT_HPP

#include "src/elements/movable/falling/FallingElement.hpp"

/**
 * @brief Represents a liquid element with buoyancy and horizontal spreading.
 * 
 * Inherits from FallingElement and BuoyantElement to provide gravity, movement,
 * and density-based interactions.
 */
class LiquidElement : public FallingElement {
public:
	/**
	 * @brief Construct a new LiquidElement.
	 * @param type The element type.
	 * @param x Initial x-coordinate.
	 * @param y Initial y-coordinate.
	 */
	LiquidElement(ElementType type, int x, int y) : FallingElement(type, x, y) {}

	/**
	 * @brief Update the liquid element for the simulation step.
	 * @param matrix The simulation matrix.
	 */
	void update(IMatrix& matrix) override;

protected:
	/**
	 * @brief Determine if this liquid can swap with the element at (x, y).
	 * @param matrix The simulation matrix.
	 * @param x The x-coordinate to check.
	 * @param y The y-coordinate to check.
	 * @return True if swap is allowed, false otherwise.
	 */
	bool canSwapWithElement(IMatrix& matrix, int x, int y) const override;

	/**
	 * @brief Handles horizontal spreading and dispersion of the liquid.
	 * @param matrix The simulation matrix.
	 */
	void handleHorizontalSpreading(IMatrix& matrix);

	void recieveNeighborEffect() override;
	void handleGrounded(IMatrix& matrix) override;

	void handleBuoyancy(IMatrix& matrix) override;

	/**
	 * @brief The maximum distance the liquid can spread horizontally per update.
	 */
	int m_DispersionRate = 32;
};

#endif // LIQUID_ELEMENT_HPP
