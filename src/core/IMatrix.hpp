// src/elements/IMatrix.hpp
#ifndef IMATRIX_HPP
#define IMATRIX_HPP

#include "src/elements/ElementFactory.hpp"

// Forward declaration
class Element;

//-------------------------------------------
// Matrix Access Interface
//-------------------------------------------
// Interface that provides controlled access to matrix operations
// without exposing the full CellularMatrix implementation
//-------------------------------------------
class IMatrix {
public:
	virtual ~IMatrix() = default;
	
	// Bounds checking
	virtual bool isInBounds(int x, int y) const = 0;
	
	// Type checking
	virtual bool isEmpty(int x, int y) const = 0;

	// Element management
	virtual Element*& getElement(int x, int y) = 0;
	virtual const Element* getElement(int x, int y) const = 0;
	virtual void placeElement(int x, int y, ElementType type) = 0;
	virtual void destroyElement(int x, int y) = 0;
	virtual void swapElements(int x1, int y1, int x2, int y2) = 0;

	// Chunk Management 
	virtual void activateChunk(int x, int y) = 0;
};

#endif // IMATRIX_HPP