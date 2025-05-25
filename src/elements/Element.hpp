// -------------------------------------------
// Element.hpp
// -------------------------------------------
// Base class for all elements in the simulation.
// Provides color, randomization, and interface for simulation behavior.
// -------------------------------------------
#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <algorithm>
#include "ElementType.hpp"
#include "ElementColors.hpp"

// Forward declaration to avoid circular dependency
class CellularMatrix;

class Element {
	public:
		// --- Interface ---
		Element(ElementType type, int x, int y)
			: color(ElementColors::getColorByElementType(type, x, y)),
			type(type)
		{}
		virtual ~Element() = default;
		ElementType getType() { return type; };      // Get element type
		virtual SDL_Color getColor() const { return color; } // Get element color
		virtual void update(CellularMatrix& matrix, int x, int y) = 0;

	protected:
		// --- Random Number Generation ---
		static std::mt19937 rng;      // Mersenne Twister RNG
		static int getRandomDirection();           // Get random direction (-1 or 1)

		// --- Element Color ---
		SDL_Color color = {255, 255, 255, 255}; // Default color (white)
		ElementType type = EMPTY;

		bool step = false;
};

#endif // ELEMENT_HPP
