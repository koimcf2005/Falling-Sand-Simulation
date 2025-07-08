// src/elements/liquids/Liquid.hpp
#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "src/elements/Element.hpp"

/**
 * @brief Abstract base class for all liquid elements (e.g., water, lava).
 *
 * Provides velocity, dispersion, and dissolved element behavior.
 * This class is not meant to be instantiated directly.
 */
class Liquid : public Element {
public:
	// ========== Construction ==========
	Liquid(ElementType type, int x, int y) : Element(type, x, y) {}
	virtual ~Liquid() = default;

	// ========== Core Interface ==========
	void update(IMatrix& matrix, int x, int y) override;
	bool canReplaceElement(IMatrix& matrix, int x, int y) const override;

	// ========== Dissolved Element Accessors ==========
	ElementType getDissolvedElement() const;
	void setDissolvedElement(ElementType type);
	void setdestroyDissolvedElement(bool value);

protected:
	// ========== Physical Properties ==========
	float velocity_x = 0.0f;                ///< Horizontal velocity
	float accumulated_x = 0.0f;             ///< Horizontal movement accumulator
	int dispersionRate = 10;                ///< Max distance a liquid can spread
	ElementType dissolvedElement = EMPTY;   ///< Currently dissolved element (if any)
	bool destroyDissolvedElement = false;   ///< Should remove dissolved element on next update

	// ========== Motion & Interaction ==========
	void handleHorizontalSpreading(IMatrix& matrix, int x, int y);
	void handleBuoyancy(IMatrix& matrix, int x, int y) override;
	void applyAdjacentNeighborEffect() override;
	void handleWhileGrounded(IMatrix& matrix, int x, int y) override;

	// ========== Diffusion ==========
	void diffuseDissolvedElement(IMatrix& matrix, int x, int y);
};

#endif // LIQUID_HPP
