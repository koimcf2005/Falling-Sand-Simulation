#ifndef CELLULAR_MATRIX_HPP
#define CELLULAR_MATRIX_HPP

#include "./elements/Element.hpp"
#include "./elements/ElementColors.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <thread>

//-------------------------------------------
// Cellular Matrix Class
//-------------------------------------------
// Main simulation grid that:
// 1. Manages a 2D grid of elements
// 2. Handles element updates with physics
// 3. Renders elements to screen efficiently
//-------------------------------------------
class CellularMatrix {
public:
	static bool step;
	//-------------------------------------------
	// Construction/Destruction
	//-------------------------------------------
	CellularMatrix(int width, int height);
	~CellularMatrix();

	//-------------------------------------------
	// Grid Properties
	//-------------------------------------------
	std::vector<std::vector<Element*>> matrix;		// 2D grid of elements

	//-------------------------------------------
	// Element Management
	//-------------------------------------------
	// Place a single element at specified coordinates
	void placeElement(int x, int y, ElementType type);

	// Place elements in a rectangular area (for brush tool)
	void placeElementsInArea(int startX, int startY, int radius, ElementType type);

	bool isInBounds(int x, int y);

	bool isEmpty(int x, int y);

	Element*& getElement(int x, int y);

	//-------------------------------------------
	// Simulation Update
	//-------------------------------------------
	// Update all elements in the grid (with randomized order)
	void update();

	// Helper for parallel updates (currently unused)
	static void updateColumnRange(int startCol, int step, int colWidth, 
								std::vector<std::vector<Element*>>& matrix, 
								int width, int height);

	//-------------------------------------------
	// Rendering
	//-------------------------------------------
	// Initialize the rendering texture
	void initializeRenderer(SDL_Renderer* renderer);

	// Render the current state to screen
	void render(SDL_Renderer* renderer);

private:
	const int WIDTH, HEIGHT;						  // Grid dimensions

	std::mt19937 rng{std::random_device{}()};

	//-------------------------------------------
	// Rendering Properties
	//-------------------------------------------
	SDL_Texture* renderTexture = nullptr;			 // Hardware accelerated texture
	std::vector<Uint32> pixels;					   // Pixel buffer for efficient updates

};

#endif // CELLULAR_MATRIX_HPP