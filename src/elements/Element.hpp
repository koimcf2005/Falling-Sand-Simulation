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
#include <unordered_map>
#include "ElementType.hpp"
#include "ElementColors.hpp"

// Forward declaration to avoid circular dependency
class CellularMatrix;

class Element {
public:
	// --- Construction/Destruction ---
	Element(ElementType type, int x, int y);
	virtual ~Element() = default;

	// --- Core Interface ---
	ElementType getType() const;
	virtual void update(CellularMatrix& matrix, int x, int y) = 0;

	// --- Step Flag ---
	bool getStep() const;
	void setStep(bool value);

	// --- Color ---
	SDL_Color getColor() const;
	void setColor(const SDL_Color& newColor);

	float getDensity() const;

protected:
	// --- Random Number Generation ---
	static std::mt19937 rng; // Mersenne Twister RNG
	static std::uniform_real_distribution<float> norm_dist;
	static int getRandomDirection(); // Get random direction (-1 or 1)

	// --- Update Helper ---
	bool checkIfUpdated();
	bool step = false; // Used to prevent double-updating in a single frame

	// --- Element Management ---
	static void swapElement(CellularMatrix& matrix, int x1, int y1, int x2, int y2);
	static void destroyElement(CellularMatrix& matrix, int x, int y);

	// --- Neighbor/Type Utilities ---
	static int isInsideElement(CellularMatrix& matrix, ElementType type, int x, int y);
	// Type checking utility (like Java's instanceof)
	template<typename T>
	static bool isInstanceOf(Element* element) {
		return dynamic_cast<T*>(element) != nullptr;
	}

	// --- Dissolution ---
	// List of elements this can be dissolved in, mapped to chance
	std::unordered_map<ElementType, float> dissolveChances;
	int dissolveThreshold = 1;

	// --- Element Properties ---
	SDL_Color color = {255, 255, 255, 255}; // Default color (white)
	SDL_Color backup_color = {255, 255, 255, 255};
	ElementType type = EMPTY;

	float density = 0.5f;

	// Flag indicating whether the solid is free falling
	bool wasMoving = true;
	bool isMoving = true;

	// Returns a vector of pointers to neighboring elements of the specified type
	template<typename T>
	static std::vector<T*> getNeighborElementsOfType(CellularMatrix& matrix, ElementType type, int x, int y) {
		std::vector<T*> neighbors;
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 && dy == 0) continue; // Skip the center cell
				int nx = x + dx;
				int ny = y + dy;
				if (matrix.isInBounds(nx, ny)) {
					Element* elem = matrix.getElement(nx, ny);
					if (elem && elem->getType() == type) {
						T* casted = dynamic_cast<T*>(elem);
						if (casted) {
							neighbors.push_back(casted);
						}
					}
				}
			}
		}
		return neighbors;
	}
};

#endif // ELEMENT_HPP
