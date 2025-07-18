// src/elements/Element.hpp
#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_map>
#include "src/elements/ElementFactory.hpp"
#include "src/core/IMatrix.hpp"

/**
 * @brief Base class for all elements in the simulation.
 */
class Element {
public:
	// ========= Construction & Destruction =========
	Element(ElementType type, int x, int y);
	virtual ~Element() = default;

	// ========= Core Update Interface =========
	virtual void update(IMatrix& matrix, int x, int y) = 0;

	// ========= Element Metadata =========
	ElementType getType() const;
	std::string getTypeString() const;
	SDL_Color getColor() const;
	void setColor(const SDL_Color& newColor);

	// ========= Simulation Step Tracking =========
	bool getHasUpdated() const;
	void setAsUpdated();
	void setAsMoving();
	static bool globalStep;

	// ========= Physical Properties =========
	float getVelocityY() const;
	void addVelocityY(float value);
	void setVelocityY(float value);
	float getDensity() const;
	float getFriction() const;
	float getImpactAbsorption() const;
	float getInertialResistance() const;

	// ========= Temperature System =========
	float getTemperature() const;
	void setTemperature(float temp);
	void addTemperature(float temp);
	float getTemperatureThreshold() const;
	virtual void reactToHeat(IMatrix& matrix, int x, int y);
	virtual void reactToCooling(IMatrix& matrix, int x, int y);
	
protected:
	// ========= Movement Logic =========
	virtual void handleFalling(IMatrix& matrix, int x, int y);
	virtual void handleBuoyancy(IMatrix& matrix, int x, int y);
	virtual void handleWhileGrounded(IMatrix& matrix, int x, int y); // Called every frame element is resting
	virtual void affectAdjacentNeighbors(IMatrix& matrix, int x, int y);
	virtual void applyAdjacentNeighborEffect();
	virtual bool canReplaceElement(IMatrix& matrix, int x, int y) const;

	float calculateAbsorption(IMatrix& matrix, int x1, int y1, int x2, int y2);
	float calculateFriction(IMatrix& matrix, int x1, int y1, int x2, int y2);
	
	// ========= Update Management =========
	bool checkIfUpdated();
	bool handleHeat(IMatrix& matrix, int x, int y);

	// ========= Element Management =========
	static void swapElements(IMatrix& matrix, int x1, int y1, int x2, int y2);
	static void destroyElement(IMatrix& matrix, int x, int y);

	// ========= RNG Utilities =========
	static std::mt19937 rng;
	static std::uniform_real_distribution<float> norm_dist;
	static int getRandomDirection();

	// ========= Template Helpers =========
	template<typename T>
	static bool isInstanceOf(Element* element) {
		return dynamic_cast<T*>(element) != nullptr;
	}

	template<typename T>
	static std::vector<T*> getNeighborElementsOfType(IMatrix& matrix, ElementType type, int x, int y);

	// ========= State Fields =========
	ElementType type = EMPTY;

	// Color
	SDL_Color color = {255, 255, 255, 255};
	SDL_Color backup_color = {255, 255, 255, 255};

	// Physics
	const float GRAVITY = 0.1f;
	float density = 0.5f;
	float velocity_y = 1.0f;
	float accumulated_y = 0.0f;
	float friction = 0.1f;
	float impactAbsorption = 0.5f;
	float inertialResistance = 0.5f;

	// Temperature System
	float temperature = 20.0f; // Room temperature in Celsius
	float temperatureThreshold = 100.0f; // Default threshold
	bool shouldReactToHeat = true;

	// Movement Flags
	bool wasMoving = true;
	bool isMoving = true;
	bool movedThisFrame = false;
	bool step = false;

	// Dissolution
	std::unordered_map<ElementType, float> dissolveChances;
	int dissolveThreshold = 1;
};

template<typename T>
std::vector<T*> Element::getNeighborElementsOfType(IMatrix& matrix, ElementType type, int x, int y) {
	std::vector<T*> neighbors;
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			int nx = x + dx;
			int ny = y + dy;
			if (matrix.isInBounds(nx, ny)) {
				Element* elem = matrix.getElement(nx, ny);
				if (elem && elem->getType() == type) {
					if (T* casted = dynamic_cast<T*>(elem)) {
						neighbors.push_back(casted);
					}
				}
			}
		}
	}
	return neighbors;
}

#endif // ELEMENT_HPP