#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

#include "src/core/Globals.hpp"
#include "src/elements/Element.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/core/CellularMatrix.hpp"
#include "src/core/Renderer.hpp"
#include "src/particles/ParticleManager.hpp"
#include "src/ui/ElementUI.hpp"
#include "src/ui/DebugUI.hpp"

//-------------------------------------------
// Function Prototypes
//-------------------------------------------
bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer);
void handleEvents(bool& running, SDL_Event& event, ElementUI& elementUI, bool& leftMouseDown, bool& rightMouseDown, int& areaSize, CellularMatrix& matrix);
void handleElementPlacement(CellularMatrix& matrix, int mouseX, int mouseY, bool& leftMouseDown, bool& rightMouseDown, int& prevGridX, int& prevGridY, int areaSize, ElementType selectedElement, bool mouseOverUI);

//-------------------------------------------
// Entry Point
//-------------------------------------------
int main() {
	// Load all element types
	ElementFactory::initialize();

	// Initialize the global renderer pointer before using it
	g_Renderer = new Renderer();

	if (!g_Renderer->initialize("Falling Sand Simulation")) {
		std::cerr << "Renderer init failed\n";
		return -1;
	}
	g_Renderer->setLogicalResolution();

	// Initialize simulation
	CellularMatrix matrix(Matrix::WIDTH, Matrix::HEIGHT);
	matrix.initializeTexture(g_Renderer->getRenderer());

	//-------------------------------------------
	// Simulation State Variables
	//-------------------------------------------
	ElementType selectedElement = SAND;
	int areaSize = 3;
	bool running = true, leftMouseDown = false, rightMouseDown = false;
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
			int totalChunks = ((Matrix::WIDTH + g_CHUNK_SIZE - 1) 
							  / g_CHUNK_SIZE)
							  * ((Matrix::HEIGHT + g_CHUNK_SIZE - 1)
							  / g_CHUNK_SIZE);
			g_Renderer->getDebugUI()->update(currentTime, activeChunks, totalChunks);
		}

		// Switch to window coordinates for UI/event handling
		g_Renderer->resetLogicalResolution();

		// Handle all SDL events (keyboard, mouse, etc.)
		while (SDL_PollEvent(&event)) {
			handleEvents(running, event, *g_Renderer->getElementUI(), leftMouseDown, rightMouseDown, areaSize, matrix);
		}

		// Update UI and retrieve current selected element
		selectedElement = g_Renderer->getElementUI()->getSelectedElement();

		// Get current mouse position in window (screen) coordinates
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// Update UI with window coordinates (not logical)
		g_Renderer->getElementUI()->update(mouseX, mouseY);

		// Determine if mouse is hovering over UI (window coordinates)
		bool mouseOverUI = g_Renderer->getElementUI()->isMouseOverUI(mouseX, mouseY);

		// For simulation/brush, still use logical coordinates
		int logicalMouseX = mouseX * Matrix::WIDTH / Window::WIDTH;
		int logicalMouseY = mouseY * Matrix::HEIGHT / Window::HEIGHT;

		// Handle brush placement if mouse is held down
		handleElementPlacement(matrix, mouseX, mouseY, leftMouseDown, rightMouseDown, prevGridX, prevGridY, areaSize, selectedElement, mouseOverUI);

		// Fixed timestep physics updates
		while (lag >= g_MS_PER_UPDATE) {
			matrix.update();
			lag -= g_MS_PER_UPDATE;
		}

		//-------------------------------------------
		// Rendering
		//-------------------------------------------
		g_Renderer->renderScene(matrix, matrix.getDebugMode());
		g_Renderer->drawBrushOutline(logicalMouseX, logicalMouseY, areaSize, mouseOverUI);
		g_Renderer->present();
	}

	// Cleanup and shutdown
	g_Renderer->cleanup();
	delete g_Renderer;
	g_Renderer = nullptr;
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
	window = SDL_CreateWindow("Cellular Matrix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window::WIDTH, Window::HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) return false;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	return renderer != nullptr;
}

//-------------------------------------------
// Input & UI Event Handling
//-------------------------------------------
void handleEvents(bool& running, SDL_Event& event, ElementUI& elementUI, bool& leftMouseDown, bool& rightMouseDown, int& areaSize, CellularMatrix& matrix) {
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
void handleElementPlacement(CellularMatrix& matrix, int mouseX, int mouseY, bool& leftMouseDown, bool& rightMouseDown, int& prevGridX, int& prevGridY, int areaSize, ElementType selectedElement, bool mouseOverUI) {
	if (!(leftMouseDown || rightMouseDown) || mouseOverUI) {
		prevGridX = prevGridY = -1;
		return;
	}

	// Convert mouse to grid coords
	int gridX = mouseX * Matrix::WIDTH / Window::WIDTH;
	int gridY = mouseY * Matrix::HEIGHT / Window::HEIGHT;
	if (gridX < 0 || gridX >= Matrix::WIDTH || gridY < 0 || gridY >= Matrix::HEIGHT) return;

	// Always place at current mouse position
	matrix.placeElementsInArea(gridX, gridY, areaSize, leftMouseDown ? selectedElement : EMPTY);

	// Interpolate between frames to prevent brush skipping on fast movement
	if (prevGridX != -1 && prevGridY != -1) {
		int dx = gridX - prevGridX;
		int dy = gridY - prevGridY;
		int steps = std::max(std::abs(dx), std::abs(dy));
		for (int i = 1; i <= steps; ++i) {
			int interpX = prevGridX + dx * i / steps;
			int interpY = prevGridY + dy * i / steps;
			matrix.placeElementsInArea(interpX, interpY, areaSize, leftMouseDown ? selectedElement : EMPTY);
		}
	}

	prevGridX = gridX;
	prevGridY = gridY;
}
