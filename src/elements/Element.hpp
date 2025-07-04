// src/elements/Element.hpp
#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_map>
#include "ElementType.hpp"
#include "ElementColors.hpp"
#include "../IMatrix.hpp"

/**
 * @brief Abstract base class for all elements in the simulation.
 * 
 * Provides core interface, properties, and utility functions for derived elements.
 */
class Element {
public:
	// ==================== Construction/Destruction ====================

	/**
	* @brief Construct a new Element.
	* @param type The type of the element.
	* @param x Initial x position.
	* @param y Initial y position.
	*/
	Element(ElementType type, int x, int y);

	/**
	* @brief Virtual destructor.
	*/
	virtual ~Element() = default;

	// ==================== Core Interface ====================

	/**
	* @brief Get the type of this element.
	* @return ElementType
	*/
	ElementType getType() const;

	/**
	* @brief Update the element's state for one simulation step.
	* @param matrix Reference to the simulation matrix.
	* @param x Current x position.
	* @param y Current y position.
	*/
	virtual void update(IMatrix& matrix, int x, int y) = 0;

	// ==================== Update State ====================

	/**
	* @brief Check if this element has already updated this step.
	* @return true if updated, false otherwise.
	*/
	bool getHasUpdated() const;

	/**
	* @brief Mark this element as updated for the current step.
	*/
	void setAsUpdated();

	// ==================== Color Management ====================

	/**
	* @brief Get the current color of the element.
	* @return SDL_Color
	*/
	SDL_Color getColor() const;

	/**
	* @brief Set the color of the element.
	* @param newColor The new color.
	*/
	void setColor(const SDL_Color& newColor);

	// ==================== Physical Properties ====================

	/**
	* @brief Get the density of the element.
	* @return float
	*/
	float getDensity() const;

	// ==================== Global Step Flag ====================

	/**
	* @brief Global flag to indicate simulation step.
	*/
	static bool globalStep;

protected:
	// ==================== Random Number Generation Utilities ====================

	/**
	* @brief Random number generator for element behavior.
	*/
	static std::mt19937 rng;

	/**
	* @brief Normalized float distribution [0,1).
	*/
	static std::uniform_real_distribution<float> norm_dist;

	/**
	* @brief Get a random direction (-1 or 1).
	* @return int
	*/
	static int getRandomDirection();

	// ==================== Update Helper ====================

	/**
	* @brief Check and update the step flag for this element.
	* @return true if the element should update, false otherwise.
	*/
	bool checkIfUpdated();

	/**
	* @brief Per-element step flag.
	*/
	bool step = false;

	// ==================== Element Management Utilities ====================

	/**
	* @brief Swap two elements in the matrix.
	*/
	static void swapElement(IMatrix& matrix, int x1, int y1, int x2, int y2);

	/**
	* @brief Destroy an element at the given position.
	*/
	static void destroyElement(IMatrix& matrix, int x, int y);

	// ==================== Neighbor/Type Utilities ====================

	/**
	* @brief Check if a position contains an element of a given type.
	* @return 1 if inside and matches type, 0 otherwise.
	*/
	static int isInsideElement(IMatrix& matrix, ElementType type, int x, int y);

	/**
	* @brief Check if an element is an instance of type T.
	*/
	template<typename T>
	static bool isInstanceOf(Element* element) {
		return dynamic_cast<T*>(element) != nullptr;
	}

	// ==================== Element Properties ====================

	SDL_Color color = {255, 255, 255, 255};		//< Current color.
	SDL_Color backup_color = {255, 255, 255, 255}; //< Backup color for restoration.
	ElementType type = EMPTY;					 //< Element type.
	float density = 0.5f;						 //< Density for physics.
	bool wasMoving = true;						//< Was moving last step.
	bool isMoving = true;						 //< Is moving this step.

	// ==================== Dissolution Properties ====================

	std::unordered_map<ElementType, float> dissolveChances; //< Dissolve chance per type.
	int dissolveThreshold = 1;							 //< Threshold for dissolution.

	// ==================== Neighbor Query ====================

	/**
	* @brief Get neighboring elements of a specific type, cast to T*.
	* @param matrix The simulation matrix.
	* @param type The element type to search for.
	* @param x Center x position.
	* @param y Center y position.
	* @return std::vector<T*> Vector of pointers to neighboring elements of type T.
	*/
	template<typename T>
	static std::vector<T*> getNeighborElementsOfType(IMatrix& matrix, ElementType type, int x, int y) {
		std::vector<T*> neighbors;
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 && dy == 0) continue;
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