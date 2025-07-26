#include "src/core/Globals.hpp"
#include "src/renderer/Renderer.hpp"
#include "src/renderer/ui/DebugUI.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/elements/ElementTypes.hpp"
#include "src/matrix/Matrix.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <entt/entt.hpp>
#include <iostream>

int main() {
  //-------------------------------------------
  // Initialize Game
  //-------------------------------------------  
	if (!Renderer::initialize("Falling Sand Simulation")) {
    std::cerr << "Renderer init failed\n";
		return -1;
	}
	Renderer::setSimulationResolution();
  
  ElementFactory::initialize();

  Matrix* matrix = new Matrix();

  matrix->initializeTexture(Renderer::getRenderer());

  //-------------------------------------------
  // Simulation State Variables
  //-------------------------------------------
  ElementType selected_element = SAND;
  int area_size = 3;
  bool running = true, left_mouse_down = false, right_mouse_down = false;
  bool debug_mode = false;
  int prevGridX = -1, prevGridY = -1;
  SDL_Event event;
  Uint32 current_time = SDL_GetTicks(), previous_time = current_time;
  float lag = 0;

	//-------------------------------------------
	// Main Loop
	//-------------------------------------------
  while (running) {
		// Fixed timestep time tracking
		current_time = SDL_GetTicks();
		float elapsed = current_time - previous_time;
		previous_time = current_time;
		lag += elapsed;

    Renderer::setWindowResolution();

    DebugUI::update(
      current_time,
      matrix->getActiveChunkCount(),
      Chunks::CHUNKS_X * Chunks::CHUNKS_Y,
      debug_mode
    );

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) left_mouse_down = true;
        if (event.button.button == SDL_BUTTON_RIGHT) right_mouse_down = true;
      }
      else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) left_mouse_down = false;
        if (event.button.button == SDL_BUTTON_RIGHT) right_mouse_down = false;
      }
      else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          // case SDLK_TAB: elementUI.toggleVisibility(); break;
          case SDLK_F1: debug_mode = !debug_mode; break;
          case SDLK_F2: matrix->toggleDebugMode(); break;
        }
      }
      else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) area_size = std::min(area_size + 1, 50);
        else if (event.wheel.y < 0) area_size = std::max(area_size - 1, 1);
      }
    }

		// Get current mouse position in window (screen) coordinates
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// For simulation/brush, still use simulation coordinates
		int simulationMouseX = mouseX * Simulation::WIDTH / Window::WIDTH;
		int simulationMouseY = mouseY * Simulation::HEIGHT / Window::HEIGHT;

    // Handle left and right mouse independently
    if (left_mouse_down) {
      matrix->placeElementsInArea(SAND, simulationMouseX, simulationMouseY, area_size);
    }
    if (right_mouse_down) {
      matrix->placeElementsInArea(EMPTY, simulationMouseX, simulationMouseY, area_size);
    }

    while (lag >= g_MS_PER_UPDATE) {
      matrix->update();
      lag -= g_MS_PER_UPDATE;
    }

    Renderer::renderScene(*matrix);
    Renderer::present();
  }

  ElementFactory::cleanup();
  Renderer::cleanup();

  return 0;
}
