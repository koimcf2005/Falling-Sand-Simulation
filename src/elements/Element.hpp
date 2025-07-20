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
#include "src/elements/utilities/rng/ElementRNG.hpp"
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
	virtual void update(IMatrix& matrix) = 0;
	
	// ========= Element Metadata =========
	ElementType getType() const;
	std::string getTypeString() const;
	SDL_Color getColor() const;
	void setColor(const SDL_Color& newColor);
	
	// ========= Simulation Step Tracking =========
	bool getHasUpdated() const;
	void setAsUpdated();
	
	// ========= Positional Properties =========
	int getPosX() const;
	int getPosY() const;
	void setPosition(int x, int y);
	
	// ========= Template Functions =========
	template<typename T>
	T* as() { return dynamic_cast<T*>(this); }
	
	template<typename T>
	const T* as() const { return dynamic_cast<const T*>(this); }
	
	// Static step
	static bool s_Step;

protected:
	// ========= Update Management =========
	bool checkIfUpdated();

	// ========= Element Management =========
	void destroyElement(IMatrix& matrix, int x, int y);
	void destroySelf(IMatrix& matrix);

	// ========= Member Variables =========
	ElementType m_Type = EMPTY;
	SDL_Color m_Color = {255, 255, 255, 255};
	SDL_Color m_OriginalColor = {255, 255, 255, 255};
	bool m_Step = false;
	int m_PosX = 0;
	int m_PosY = 0;
};

#endif // ELEMENT_HPP