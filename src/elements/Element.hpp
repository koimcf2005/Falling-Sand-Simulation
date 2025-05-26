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
		bool step = false;

		// Static utility for type checking (like Java's instanceof)
		template<typename T>
		static bool isInstanceOf(Element* element) {
			return dynamic_cast<T*>(element) != nullptr;
		}

	protected:
		// --- Random Number Generation ---
		static std::mt19937 rng;      // Mersenne Twister RNG
		static int getRandomDirection();           // Get random direction (-1 or 1)

		bool checkIfUpdated();

		static void destroyElement(CellularMatrix& matrix, int x, int y);

		// --- Element Color ---
		SDL_Color color = {255, 255, 255, 255}; // Default color (white)
		ElementType type = EMPTY;

};

#endif // ELEMENT_HPP
