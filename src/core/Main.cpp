#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "src/core/Globals.hpp"
#include "src/elements/Element.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/core/CellularMatrix.hpp"
#include "src/core/Renderer.hpp"
#include "src/ui/ElementUI.hpp"
#include "src/ui/DebugUI.hpp"

//-------------------------------------------
// Constants
//-------------------------------------------
const int WINDOW_WIDTH = 1024;        // Display window resolution (pixels)
const int WINDOW_HEIGHT = 512;
const int RENDER_WIDTH = 512;         // Internal simulation resolution
const int RENDER_HEIGHT = 256;
const float PHYSICS_HZ = 60.0f;       // Physics update frequency
const float MS_PER_UPDATE = 1000.0f / PHYSICS_HZ; // Fixed timestep (ms)

//-------------------------------------------
// Function Declarations
//-------------------------------------------
bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer);
void shutdown(SDL_Window* window, SDL_Renderer* renderer);
void handleEvents(bool& running, SDL_Event& event, ElementUI& elementUI, bool& leftMouseDown, bool& rightMouseDown, bool& heatMode, int& areaSize, CellularMatrix& matrix);
void handleElementPlacement(CellularMatrix& matrix, int mouseX, int mouseY, bool& leftMouseDown, bool& rightMouseDown, bool heatMode, int& prevGridX, int& prevGridY, int areaSize, ElementType selectedElement, bool mouseOverUI);

//-------------------------------------------
// Entry Point
//-------------------------------------------
int main() {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	// Initialize SDL and TTF
	if (!initializeSDL(window, renderer)) return -1;

	gRenderer = new Renderer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, RENDER_WIDTH, RENDER_HEIGHT);
	gRenderer->setLogicalResolution();

	// Load all element types
	ElementFactory::initialize();

	// Initialize ElementUI
	ElementUI elementUI;
	if (!elementUI.initialize(renderer, RENDER_WIDTH, RENDER_HEIGHT)) {
		std::cerr << "Failed to initialize Element UI\n";
		shutdown(window, renderer);
		return -1;
	}

	// Initialize DebugUI
	DebugUI debugUI;
	if (!debugUI.initialize(renderer)) {
		std::cerr << "Failed to initialize Debug UI\n";
		shutdown(window, renderer);
		return -1;
	}

	// Initialize simulation
	CellularMatrix matrix(RENDER_WIDTH, RENDER_HEIGHT);
	matrix.initializeRenderer(renderer);

	//-------------------------------------------
	// Simulation State Variables
	//-------------------------------------------
	ElementType selectedElement = SAND;
	int areaSize = 3;
	bool running = true, leftMouseDown = false, rightMouseDown = false, heatMode = false;
	int prevGridX = -1, prevGridY = -1;
	SDL_Event event;
	Uint32 currentTime = SDL_GetTicks(), previousTime = currentTime;
	float lag = 0;

	//-------------------------------------------
	// Main Loop
	//-------------------------------------------
	while (running) {
		// Fixed timestep time tracking
		currentTime = SDL_GetTicks();
		float elapsed = currentTime - previousTime;
		previousTime = currentTime;
		lag += elapsed;

		// Update debug overlay
		if (matrix.getDebugMode()) {
			int activeChunks = matrix.getActiveChunkCount();
			int totalChunks = ((RENDER_WIDTH + Chunk::CHUNK_SIZE - 1) 
							  / Chunk::CHUNK_SIZE)
							  * ((RENDER_HEIGHT + Chunk::CHUNK_SIZE - 1)
							  / Chunk::CHUNK_SIZE);
			debugUI.update(currentTime, activeChunks, totalChunks);
		}

		// Handle all SDL events (keyboard, mouse, etc.)
		while (SDL_PollEvent(&event)) {
			handleEvents(running, event, elementUI, leftMouseDown, rightMouseDown, heatMode, areaSize, matrix);
		}

		// Update UI and retrieve current selected element
		selectedElement = elementUI.getSelectedElement();

		// Get current mouse position and convert to simulation coordinates
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		int logicalMouseX = mouseX * RENDER_WIDTH / WINDOW_WIDTH;
		int logicalMouseY = mouseY * RENDER_HEIGHT / WINDOW_HEIGHT;
		elementUI.update(logicalMouseX, logicalMouseY);

		// Determine if mouse is hovering over UI
		bool mouseOverUI = elementUI.isMouseOverUI(logicalMouseX, logicalMouseY);

		// Handle brush placement if mouse is held down
		handleElementPlacement(matrix, mouseX, mouseY, leftMouseDown, rightMouseDown, heatMode, prevGridX, prevGridY, areaSize, selectedElement, mouseOverUI);

		// Fixed timestep physics updates
		while (lag >= MS_PER_UPDATE) {
			matrix.update();
			lag -= MS_PER_UPDATE;
		}

		//-------------------------------------------
		// Rendering
		//-------------------------------------------
		gRenderer->renderScene(matrix, elementUI, debugUI, matrix.getDebugMode());
		gRenderer->drawBrushOutline(logicalMouseX, logicalMouseY, areaSize, mouseOverUI);
		// gRenderer->drawScreenSpaceRect(100, 100, 200, 120, 1);
		gRenderer->present();
	}

	// Cleanup and shutdown
	elementUI.cleanup();
	delete gRenderer;
	gRenderer = nullptr;
	shutdown(window, renderer);
	return 0;
}

//-------------------------------------------
// SDL Initialization
//-------------------------------------------
bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init failed: " << SDL_GetError() << '\n';
		return false;
	}
	if (TTF_Init() == -1) {
		std::cerr << "TTF init failed: " << TTF_GetError() << '\n';
		return false;
	}
	window = SDL_CreateWindow("Cellular Matrix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) return false;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	return renderer != nullptr;
}

//-------------------------------------------
// SDL Cleanup
//-------------------------------------------
void shutdown(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

//-------------------------------------------
// Input & UI Event Handling
//-------------------------------------------
void handleEvents(bool& running, SDL_Event& event, ElementUI& elementUI, bool& leftMouseDown, bool& rightMouseDown, bool& heatMode, int& areaSize, CellularMatrix& matrix) {
	elementUI.handleEvent(event);

	if (event.type == SDL_QUIT) {
		running = false;
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) leftMouseDown = true;
		if (event.button.button == SDL_BUTTON_RIGHT) rightMouseDown = true;
	}
	else if (event.type == SDL_MOUSEBUTTONUP) {
		if (event.button.button == SDL_BUTTON_LEFT) leftMouseDown = false;
		if (event.button.button == SDL_BUTTON_RIGHT) rightMouseDown = false;
	}
	else if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_TAB: elementUI.toggleVisibility(); break;
			case SDLK_F1: matrix.switchDebugMode(); break;
			case SDLK_h: heatMode = !heatMode; break;
		}
	}
	else if (event.type == SDL_MOUSEWHEEL) {
		if (event.wheel.y > 0) areaSize = std::min(areaSize + 1, 20);
		else if (event.wheel.y < 0) areaSize = std::max(areaSize - 1, 1);
	}
}

//-------------------------------------------
// Element Brush Placement and Interpolation
//-------------------------------------------
void handleElementPlacement(CellularMatrix& matrix, int mouseX, int mouseY, bool& leftMouseDown, bool& rightMouseDown, bool heatMode, int& prevGridX, int& prevGridY, int areaSize, ElementType selectedElement, bool mouseOverUI) {
	if (!(leftMouseDown || rightMouseDown) || mouseOverUI) {
		prevGridX = prevGridY = -1;
		return;
	}

	// Convert mouse to grid coords
	int gridX = mouseX * RENDER_WIDTH / WINDOW_WIDTH;
	int gridY = mouseY * RENDER_HEIGHT / WINDOW_HEIGHT;
	if (gridX < 0 || gridX >= RENDER_WIDTH || gridY < 0 || gridY >= RENDER_HEIGHT) return;

	// Lambda to place or heat elements
	auto place = [&](int x, int y) {
		if (!heatMode)
			matrix.placeElementsInArea(x, y, areaSize, leftMouseDown ? selectedElement : EMPTY);
		else
			matrix.heatElementsInArea(x, y, areaSize);
	};

	// Always place at current mouse position
	place(gridX, gridY);

	// Interpolate between frames to prevent brush skipping on fast movement
	if (prevGridX != -1 && prevGridY != -1) {
		int dx = gridX - prevGridX;
		int dy = gridY - prevGridY;
		int steps = std::max(std::abs(dx), std::abs(dy));
		for (int i = 1; i <= steps; ++i) {
			int interpX = prevGridX + dx * i / steps;
			int interpY = prevGridY + dy * i / steps;
			place(interpX, interpY);
		}
	}

	prevGridX = gridX;
	prevGridY = gridY;
}
