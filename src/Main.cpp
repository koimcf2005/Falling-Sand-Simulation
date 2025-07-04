#include <SDL2/SDL.h>
#include "elements/Element.hpp"
#include "elements/ElementColors.hpp"
#include "CellularMatrix.hpp"
#include <iostream>
#include <vector>
#include <string>

//-------------------------------------------
// Constants for window and rendering
//-------------------------------------------
const int WINDOW_WIDTH = 1280;	 // Display window width
const int WINDOW_HEIGHT = 800;	 // Display window height
const int RENDER_WIDTH = 320;	  // Internal simulation width
const int RENDER_HEIGHT = 200;	 // Internal simulation height

//-------------------------------------------
// Physics simulation constants
//-------------------------------------------
const float PHYSICS_HZ = 60.0f;   // Physics updates per second
const float MS_PER_UPDATE = 1000.0f / PHYSICS_HZ;  // Milliseconds per physics update

int main(int /* unused */, char* /* unused */[]) {
	//-------------------------------------------
	// SDL Initialization
	//-------------------------------------------
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return -1;
	}

	// Create window with specified dimensions
	SDL_Window* window = SDL_CreateWindow(
		"Cellular matrix with Element System",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (!window) {
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	// Create hardware-accelerated renderer with vsync
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!renderer) {
		std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	ElementColors::initialize(renderer);

	// Set up renderer properties for pixel-perfect scaling
	SDL_RenderSetLogicalSize(renderer, RENDER_WIDTH, RENDER_HEIGHT);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");  // Use nearest neighbor scaling

	//-------------------------------------------
	// Simulation Setup
	//-------------------------------------------
	// Initialize the cellular matrix system
	CellularMatrix matrix(RENDER_WIDTH, RENDER_HEIGHT);
	matrix.initializeRenderer(renderer);

	// Initialize simulation variables
	ElementType selectedElement = SAND;
	int areaSize = 3;  // Brush size for element placement

	//-------------------------------------------
	// Game Loop Variables
	//-------------------------------------------
	bool running = true;
	SDL_Event event;
	
	// Timing variables for fixed timestep updates
	Uint32 currentTime = SDL_GetTicks();
	Uint32 previousTime = currentTime;
	float lag = 0.0f;

	// FPS calculation variables
	Uint32 fpsLastTime = currentTime;
	int frameCount = 0;
	float fps = 0;

	// Input state tracking
	bool leftMouseDown = false;
	bool rightMouseDown = false;
	int prevGridX = -1, prevGridY = -1;

	//-------------------------------------------
	// Main Game Loop
	//-------------------------------------------
	while (running) {
		// Update timing for fixed timestep
		currentTime = SDL_GetTicks();
		float elapsed = currentTime - previousTime;
		previousTime = currentTime;
		lag += elapsed;

		//----------------
		// Event Handling
		//----------------
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			// Track mouse button state
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) leftMouseDown = true;
				if (event.button.button == SDL_BUTTON_RIGHT) rightMouseDown = true;
			}
			else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) leftMouseDown = false;
				if (event.button.button == SDL_BUTTON_RIGHT) rightMouseDown = false;
			}
			// Handle keyboard input for Element selection
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_1:
						selectedElement = SAND;
						std::cout << "Selected Sand\n";
						break;
					case SDLK_2:
						selectedElement = DIRT;
						std::cout << "Selected Dirt\n";
						break;
					case SDLK_3:
						selectedElement = COAL;
						std::cout << "Selected Coal\n";
						break;
					case SDLK_4:
						selectedElement = SALT;
						std::cout << "Selected Salt\n";
						break;
					case SDLK_5:
						selectedElement = WATER;
						std::cout << "Selected Water\n";
						break;
					case SDLK_6:
						selectedElement = OIL;
						std::cout << "Selected Oil\n";
						break;
					case SDLK_7:
						selectedElement = STONE;
						std::cout << "Selected Stone\n";
						break;
					case SDLK_8:
						selectedElement = WOOD;
						std::cout << "Selected Wood\n";
						break;
					case SDLK_9:
						selectedElement = SMOKE;
						std::cout << "Selected Smoke\n";
						break;
					case SDLK_0:
						selectedElement = STEAM;
						std::cout << "Selected Steam\n";
						break;
					// Add more keys as needed for different Element types
				}
			}
			// Handle scroll wheel to change area size
			else if (event.type == SDL_MOUSEWHEEL) {
				if (event.wheel.y > 0) {  // Scroll up
					areaSize = std::min(areaSize + 1, 20);  // Cap at a reasonable maximum
				} else if (event.wheel.y < 0) {  // Scroll down
					areaSize = std::max(areaSize - 1, 1);  // Minimum size is 1
				}
				std::cout << "Area size: " << areaSize << std::endl;
			}
		}

		// Always place particles if mouse button is held down
		if (leftMouseDown || rightMouseDown) {
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			int gridX = mouseX * RENDER_WIDTH / WINDOW_WIDTH;
			int gridY = mouseY * RENDER_HEIGHT / WINDOW_HEIGHT;
			if (gridX >= 0 && gridX < RENDER_WIDTH && gridY >= 0 && gridY < RENDER_HEIGHT) {
				// Interpolate between previous and current mouse positions
				if (prevGridX != -1 && prevGridY != -1) {
					int dx = gridX - prevGridX;
					int dy = gridY - prevGridY;
					int steps = std::max(std::abs(dx), std::abs(dy));
					for (int i = 1; i <= steps; ++i) {
						int interpX = prevGridX + dx * i / steps;
						int interpY = prevGridY + dy * i / steps;
						if (areaSize > 1) {
							if (leftMouseDown) {
								matrix.placeElementsInArea(interpX, interpY, areaSize, selectedElement);
							} else if (rightMouseDown) {
								matrix.placeElementsInArea(interpX, interpY, areaSize, EMPTY);
							}
						} else {
							if (leftMouseDown) {
								matrix.placeElement(interpX, interpY, selectedElement);
							} else if (rightMouseDown) {
								matrix.placeElement(interpX, interpY, EMPTY);
							}
						}
					}
				} else {
					// First click, just place at current position
					if (areaSize > 1) {
						if (leftMouseDown) {
							matrix.placeElementsInArea(gridX, gridY, areaSize, selectedElement);
						} else if (rightMouseDown) {
							matrix.placeElementsInArea(gridX, gridY, areaSize, EMPTY);
						}
					} else {
						if (leftMouseDown) {
							matrix.placeElement(gridX, gridY, selectedElement);
						} else if (rightMouseDown) {
							matrix.placeElement(gridX, gridY, EMPTY);
						}
					}
				}
				prevGridX = gridX;
				prevGridY = gridY;
			}
		} else {
			prevGridX = -1;
			prevGridY = -1;
		}

		//----------------
		// Physics Update
		//----------------
		// Process physics updates at fixed time intervals
		while (lag >= MS_PER_UPDATE) {
			matrix.update();
			lag -= MS_PER_UPDATE;
		}

		//----------------
		// FPS Calculation
		//----------------
		frameCount++;
		if (currentTime - fpsLastTime >= 1000) {
			fps = frameCount * 1000.0f / (currentTime - fpsLastTime);
			frameCount = 0;
			fpsLastTime = currentTime;
			
			std::string title = "Cellular matrix with Element System - FPS: " 
							 + std::to_string(static_cast<int>(fps));
			SDL_SetWindowTitle(window, title.c_str());
		}

		//----------------
		// Rendering
		//----------------
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		matrix.render(renderer);
		SDL_RenderPresent(renderer);
	}

	//-------------------------------------------
	// Cleanup
	//-------------------------------------------
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
