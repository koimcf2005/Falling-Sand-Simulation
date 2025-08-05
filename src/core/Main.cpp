// Main.cpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// Implements the main entry point, game loop, and input handling for the
// Falling Sand Simulation.

#include "falling-sand-sim/core/Globals.hpp"
#include "falling-sand-sim/core/Input.hpp"
#include "falling-sand-sim/components/EnTTManager.hpp"
#include "falling-sand-sim/elements/ElementFactory.hpp"
#include "falling-sand-sim/elements/ElementTypes.hpp"
#include "falling-sand-sim/renderer/Renderer.hpp"
#include "falling-sand-sim/renderer/DisplayTexture.hpp"
#include "falling-sand-sim/renderer/SystemStatsDisplay.hpp"
#include "falling-sand-sim/world/Matrix.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <entt/entt.hpp>
#include <fmt/core.h>
#include <iostream>

// ANSI color codes for terminal output
#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"

void printElementDebugInfo(
  Matrix *matrix,
  int simulation_mouse_x,
  int simulation_mouse_y
);

//-------------------------------------------
// Main Function
//-------------------------------------------
int main() {
  //-------------------------------------------
  // Game Initialization
  //-------------------------------------------

  // Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cerr << "SDL init failed: " << SDL_GetError() << '\n';
			return false;
	}
  // Initialize Input Handling
  Input::initialize();
  // Initialize Elements
  ElementFactory::initialize();
  // Initialize the Matrix
  Matrix *matrix = new Matrix();
  // Initialize the Renderer
  if (!Renderer::initialize("Falling Sand Simulation")) {
    std::cerr << "Renderer init failed\n";
    return -1;
  }
  // Initialize the Texture (Display)
  SimulationTexture *simulation_texture = new SimulationTexture();
  simulation_texture->initializeTexture(Renderer::getRenderer());

  //-------------------------------------------
  // Simulation State Variables
  //-------------------------------------------
  SDL_Event event;
  bool running = true;
  bool paused = false;

  uint8_t brush_size = 3;
  int simulation_mouse_x;
  int simulation_mouse_y;

  bool show_stats = false;
  bool debug_mode = false;
  
  float lag = 0;

  float zoom = 1.0f;

  #ifdef DEBUG_MODE
    show_stats = true;
    debug_mode = true;
    simulation_texture->toggleShowChunks();
    matrix->toggleDebugMode();
  #endif

  //-------------------------------------------
  // Main Loop
  //-------------------------------------------
  while (running) {
    Uint32 frame_start = SDL_GetTicks();

    // Accumulate lag based on elapsed time
    static Uint32 prev_ticks = frame_start;
    Uint32 now = SDL_GetTicks();
    lag += now - prev_ticks;
    prev_ticks = now;

    //-------------------------------------------
    // Update
    //-------------------------------------------
    while (lag >= Simulation::MS_PER_UPDATE) {
      //-------------------------------------------
      // Input
      //-------------------------------------------
      Input::updateState();

      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        Input::handleSDLEvent(event);
      }
      
      if (Input::getKeyDown("ESCAPE")) {
        paused = !paused;
      }
      if (Input::getKeyDown("F1")) {
        show_stats = !show_stats;
      }
      if (Input::getKeyDown("F2")) {
        simulation_texture->toggleShowChunks();
      }
      if (Input::getKeyDown("F3")) {
        debug_mode = !debug_mode;
        matrix->toggleDebugMode();
      }

      // For simulation/brush, still use simulation coordinates
      int mouse_x = Input::getMouseX();
      int mouse_y = Input::getMouseY();
      simulation_mouse_x = mouse_x / zoom * Simulation::WIDTH / Window::WIDTH;
      simulation_mouse_y = mouse_y / zoom * Simulation::HEIGHT / Window::HEIGHT;   

      if (Input::getScrollDelta() > 0) {
        brush_size = std::min(brush_size + 1, 50);
      }
      else if (Input::getScrollDelta() < 0) {
        brush_size = std::max(brush_size - 1, 1);
      }   
        
      if (Input::getMouse("LEFT")) {
        matrix->placeElementsInArea(SAND, simulation_mouse_x, simulation_mouse_y, brush_size);
      }
      if (Input::getMouse("RIGHT")) {
        matrix->placeElementsInArea(EMPTY, simulation_mouse_x, simulation_mouse_y, brush_size);
      }
      if (Input::getKeyDown("I")) {
        printElementDebugInfo(matrix, simulation_mouse_x, simulation_mouse_y);
      }

      if (debug_mode) {
        if (Input::getKeyDown("H") || Input::getKey("H", 10, 1)) {
          matrix->updateCellByCell();
        }
        else if (Input::getKeyDown("J") || Input::getKey("J", 10, 1)) {
          matrix->updateCellByCell(Simulation::WIDTH);
        }
        else if (Input::getKeyDown("K") || Input::getKey("K", 10, 1)) {
          matrix->updateCellByCell(Simulation::WIDTH * Simulation::HEIGHT);
        }
      }
 
      //-------------------------------------------
      // Update Matrix
      //-------------------------------------------     
      if (!debug_mode && !paused) matrix->update();

      //-------------------------------------------
      // Render
      //-------------------------------------------
      SystemStatsDisplay::incrementTickCount();
      Renderer::drawCircle(simulation_mouse_x, simulation_mouse_y, brush_size);
      Renderer::renderScene(*matrix, *simulation_texture);
      Renderer::present();

      //-------------------------------------------
      // End Loop
      //-------------------------------------------     
      lag -= Simulation::MS_PER_UPDATE;
    }
    
    // Increment frame count for stats
    SystemStatsDisplay::incrementFrameCount();
    SystemStatsDisplay::update(frame_start, show_stats);
  }

  ElementFactory::cleanup();
  Renderer::cleanup();

  std::exit(0);
}

/**
 * @brief Prints debug information about an element at the given coordinates.
 */
void printElementDebugInfo(Matrix *matrix, int simulation_mouse_x, int simulation_mouse_y)
{
  entt::entity entity = matrix->getEntity(simulation_mouse_x, simulation_mouse_y);
  const Element &element = EnTTManager::getComponent<Element>(entity);
  const Velocity *velocity = EnTTManager::getComponentIfExists<Velocity>(entity);
  const MovementState *movement_state = EnTTManager::getComponentIfExists<MovementState>(entity);
  const Health *health = EnTTManager::getComponentIfExists<Health>(entity);
  const Temperature *temperature = EnTTManager::getComponentIfExists<Temperature>(entity);

  fmt::print(COLOR_GREEN "\nElement Type: {}" COLOR_RESET "\n", ElementFactory::getElementName(element.type));
  fmt::print(COLOR_GREEN "  Position: " COLOR_CYAN "({}, {})" COLOR_RESET "\n", simulation_mouse_x, simulation_mouse_y);

  fmt::print(COLOR_GREEN "  Color: " COLOR_CYAN "({}, {}, {}, {})" COLOR_RESET "\n",
    element.color.r,
    element.color.g,
    element.color.b,
    element.color.a
  );

  if (!velocity)
    fmt::print(COLOR_GREEN "  Velocity: " COLOR_RED "none" COLOR_RESET "\n");
  else
    fmt::print(COLOR_GREEN "  Velocity: " COLOR_CYAN "v({}, {}), a({}, {})" COLOR_RESET "\n", velocity->vx, velocity->vy, velocity->ax, velocity->ay);

  if (!temperature)
    fmt::print(COLOR_GREEN "  Temperature: " COLOR_RED "none" COLOR_RESET "\n");
  else
    fmt::print(COLOR_GREEN "  Temperature: " COLOR_CYAN "{}/{}" COLOR_RESET "\n", temperature->temperature, temperature->threshold);

  if (!health)
    fmt::print(COLOR_GREEN "  Health: " COLOR_RED "none" COLOR_RESET "\n");
  else
    fmt::print(COLOR_GREEN "  Health: " COLOR_CYAN "{}/{}" COLOR_RESET "\n", health->health, health->max);

  if (!movement_state)
    fmt::print(COLOR_GREEN "  Movement State: " COLOR_RED "none" COLOR_RESET "\n");
  else {
    fmt::print(COLOR_GREEN "  Was Moving: " COLOR_CYAN "{}" COLOR_RESET "\n", movement_state->wasMoving() ? "true" : "false");
    fmt::print(COLOR_GREEN "  Is Moving: " COLOR_CYAN "{}" COLOR_RESET "\n", movement_state->isMoving() ? "true" : "false");
    fmt::print(COLOR_GREEN "  Moved This Frame: " COLOR_CYAN "{}" COLOR_RESET "\n", movement_state->movedThisFrame() ? "true" : "false");
  }
}
