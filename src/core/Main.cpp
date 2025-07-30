#include "src/core/Globals.hpp"
#include "src/renderer/Renderer.hpp"
#include "src/renderer/ui/DebugUI.hpp"
#include "src/elements/ElementFactory.hpp"
#include "src/elements/ElementTypes.hpp"
#include "src/matrix/Matrix.hpp"
#include "src/renderer/SimulationTexture.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <entt/entt.hpp>
#include <iostream>

//-------------------------------------------
// Function Declarations
//-------------------------------------------
void handleEvents(SDL_Event& event, bool& running, bool& left_mouse_down, bool& right_mouse_down, bool& middle_mouse_down, bool& show_stats, bool& debug_mode, SimulationTexture* simulation_texture, Matrix* matrix, int& area_size);
void handleMouseInput(Matrix* matrix, int simulation_mouse_x, int simulation_mouse_y, int area_size, bool& left_mouse_down, bool& right_mouse_down, bool& middle_mouse_down, bool debug_mode);
void printElementDebugInfo(Matrix* matrix, int simulation_mouse_x, int simulation_mouse_y);

//-------------------------------------------
// Main Function
//-------------------------------------------
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
  SimulationTexture* simulation_texture = new SimulationTexture();

  simulation_texture->initializeTexture(Renderer::getRenderer());

  //-------------------------------------------
  // Simulation State Variables
  //-------------------------------------------
  int area_size = 3;
  bool running = true;
  bool left_mouse_down = false, right_mouse_down = false, middle_mouse_down = false;
  bool show_stats = false;
  bool debug_mode = false;
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
    DebugUI::update(current_time, show_stats);

    handleEvents(event, running, left_mouse_down, right_mouse_down, middle_mouse_down, show_stats, debug_mode, simulation_texture, matrix, area_size);

		// Get current mouse position in window (screen) coordinates
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// For simulation/brush, still use simulation coordinates
		int simulation_mouse_x = mouseX * Simulation::WIDTH / Window::WIDTH;
		int simulation_mouse_y = mouseY * Simulation::HEIGHT / Window::HEIGHT;

    handleMouseInput(matrix, simulation_mouse_x, simulation_mouse_y, area_size, left_mouse_down, right_mouse_down, middle_mouse_down, debug_mode);

    if (!debug_mode) {
      while (lag >= g_MS_PER_UPDATE) {
        matrix->update();
        lag -= g_MS_PER_UPDATE;
      }
    }

    Renderer::renderScene(*matrix, *simulation_texture);
    Renderer::present();
  }

  ElementFactory::cleanup();
  Renderer::cleanup();

  std::exit(0);
}

//-------------------------------------------
// Function Definitions
//-------------------------------------------

/**
 * @brief Handles SDL events such as mouse and keyboard input.
 */
void handleEvents(SDL_Event& event, bool& running, bool& left_mouse_down, bool& right_mouse_down, bool& middle_mouse_down, bool& show_stats, bool& debug_mode, SimulationTexture* simulation_texture, Matrix* matrix, int& area_size) {
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = false;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
      if (event.button.button == SDL_BUTTON_LEFT) left_mouse_down = true;
      if (event.button.button == SDL_BUTTON_RIGHT) right_mouse_down = true;
      if (event.button.button == SDL_BUTTON_MIDDLE) middle_mouse_down = true;
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
      if (event.button.button == SDL_BUTTON_LEFT) left_mouse_down = false;
      if (event.button.button == SDL_BUTTON_RIGHT) right_mouse_down = false;
      if (event.button.button == SDL_BUTTON_MIDDLE) middle_mouse_down = false;
    }
    else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_F1: show_stats = !show_stats; break;
        case SDLK_F2: simulation_texture->toggleShowChunks(); break;
        case SDLK_F3: 
          debug_mode = !debug_mode;
          matrix->toggleDebugMode();
          break;
        case SDLK_1: 
          if (debug_mode) { matrix->updateCellByCell(); } 
          break;
        case SDLK_2: 
          if (debug_mode) { matrix->updateCellByCell(Simulation::WIDTH); } 
          break;
        case SDLK_3: 
          if (debug_mode) { matrix->updateCellByCell(Simulation::WIDTH * Simulation::HEIGHT); } 
          break;
      }
    }
    else if (event.type == SDL_MOUSEWHEEL) {
      if (event.wheel.y > 0) area_size = std::min(area_size + 1, 50);
      else if (event.wheel.y < 0) area_size = std::max(area_size - 1, 1);
    }
  }
}

/**
 * @brief Handles mouse input for placing elements and debugging.
 */
void handleMouseInput(Matrix* matrix, int simulation_mouse_x, int simulation_mouse_y, int area_size, bool& left_mouse_down, bool& right_mouse_down, bool& middle_mouse_down, bool debug_mode) {
  if (left_mouse_down) {
    matrix->placeElementsInArea(SAND, simulation_mouse_x, simulation_mouse_y, area_size);
  }
  if (right_mouse_down) {
    matrix->placeElementsInArea(EMPTY, simulation_mouse_x, simulation_mouse_y, area_size);
  }
  if (middle_mouse_down && debug_mode) {
    printElementDebugInfo(matrix, simulation_mouse_x, simulation_mouse_y);
    middle_mouse_down = false;
  }
}

/**
 * @brief Prints debug information about an element at the given coordinates.
 */
void printElementDebugInfo(Matrix* matrix, int simulation_mouse_x, int simulation_mouse_y) {
  entt::entity entity = matrix->getEntity(simulation_mouse_x, simulation_mouse_y);
  const Element& element = ComponentManager::getComponent<Element>(entity);
  const Velocity* velocity = ComponentManager::getComponentIfExists<Velocity>(entity);
  const MovementState* movement_state = ComponentManager::getComponentIfExists<MovementState>(entity);
  const Health* health = ComponentManager::getComponentIfExists<Health>(entity);
  const Temperature* temperature = ComponentManager::getComponentIfExists<Temperature>(entity);

  std::cout << "Element: " << ElementFactory::getElementName(element.type) 
            << " (" << simulation_mouse_x << ", " << simulation_mouse_y << "):\n"
            << "   Step: " << element.step << "\n"
            << "   Color: (" << static_cast<int>(element.color.r) << ", "
                          << static_cast<int>(element.color.g) << ", "
                          << static_cast<int>(element.color.b) << ", "
                          << static_cast<int>(element.color.a) << ")\n"
            << "   Velocity: (" 
            << (velocity ? std::to_string(velocity->vx) + ", " + std::to_string(velocity->vy) : "none") 
            << ")\n"
            << "   Accumulated Velocity: (" 
            << (velocity ? std::to_string(velocity->ax) + ", " + std::to_string(velocity->ay) : "none") 
            << ")\n"
            << "   Is Moving: " << (movement_state ? movement_state->isMoving() : 0) << "\n"
            << "   Moved This Frame: " << (movement_state ? movement_state->movedThisFrame() : 0) << "\n"
            << "   Health: " << (health ? std::to_string(health->health) + "/" + std::to_string(health->max) : "none") << "\n"
            << "   Temperature: " << (temperature ? std::to_string(temperature->temperature) + " (Threshold: " + std::to_string(temperature->threshold) + ")" : "none") 
            << "\n";
}
