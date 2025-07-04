// -------------------------------------------
// Liquid.hpp
// -------------------------------------------
// Base class for all liquid elements (e.g., water, lava).
// Provides velocity and movement accumulation for smooth simulation.
// Does not implement update logic directly; intended for inheritance.
// -------------------------------------------
#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "Element.hpp"

/**
 * @brief Base class for all liquid elements (e.g., water, lava).
 * 
 * Provides velocity and movement accumulation for smooth simulation.
 * Does not implement update logic directly; intended for inheritance.
 */
class Liquid : public Element {
public:
	/// ==================== Construction/Destruction ====================
	Liquid(ElementType type, int x, int y) : Element(type, x, y) {}

	/// ==================== Core Interface ====================
	void update(IMatrix& matrix, int x, int y) override;

	/// ==================== Dissolved Element Accessors ====================
	ElementType getDissolvedElement() const;
	void setDissolvedElement(ElementType type);
	void setdestroyDissolvedElement(bool value);

protected:
	/// ==================== Physical Properties ====================
	const float GRAVITY = 0.1f;        ///< Gravitational constant affecting the liquid
	float velocity_y = 1.0f;           ///< Vertical velocity
	float velocity_x = 0.0f;           ///< Horizontal velocity
	float accumulated_y = 0.0f;        ///< Accumulated vertical movement
	float accumulated_x = 0.0f;        ///< Accumulated horizontal movement
	int dispersionRate = 10;           ///< Dispersion rate for spreading
	ElementType dissolvedElement = EMPTY; ///< Dissolved element type
	bool destroyDissolvedElement = false; ///< Flag for destroying dissolved element

	/// ==================== Dissolved Element Diffusion ====================
	void diffuseDissolvedElement(IMatrix& matrix, int x, int y);

	/// ==================== Movement/Spreading Logic ====================
	bool canReplaceElementForLiquid(IMatrix& matrix, int posX, int posY) const;
	bool tryVerticalMove(IMatrix& matrix, int x, int y, int move_y);
	bool handleHorizontalSpreading(IMatrix& matrix, int x, int y);

	/// ==================== Buoyancy (Liquid-on-Liquid) ====================
	bool handleLiquidBuoyancy(IMatrix& matrix, int x, int y);

	/// ==================== Inertia Propagation ====================
	void propagateInertiaToNeighbors(IMatrix& matrix, int x, int y);
};

#endif // LIQUID_HPP