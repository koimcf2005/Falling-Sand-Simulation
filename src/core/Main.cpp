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
#include <iostream>

void printElementDebugInfo(
  Matrix *matrix,
  int simulation_mouse_x,
  int simulation_mouse_y
);

struct Input {
  const Uint8* sdl_state;
  std::vector<Uint8> current_state;
  std::vector<Uint8> last_state;
  int num_keys;

  // Mouse state
  Uint32 mouse_buttons;
  Uint32 last_mouse_buttons;
  int mouse_x, mouse_y;
  int last_mouse_x, last_mouse_y;
  int mouse_wheel_y, last_mouse_wheel_y;

  Input() {
    sdl_state = SDL_GetKeyboardState(&num_keys);
    current_state.resize(num_keys);
    last_state.resize(num_keys);
    memcpy(current_state.data(), sdl_state, num_keys);
    memcpy(last_state.data(), sdl_state, num_keys);

    mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    last_mouse_buttons = mouse_buttons;
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    mouse_wheel_y = 0;
    last_mouse_wheel_y = 0;
  }

  void updateState() {
    last_state = current_state;
    memcpy(current_state.data(), SDL_GetKeyboardState(NULL), num_keys);

    last_mouse_buttons = mouse_buttons;
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    last_mouse_wheel_y = mouse_wheel_y;

    mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    mouse_wheel_y = 0; // Reset, will be set by handleSDLEvent
  }

  // Call this from your SDL event loop for SDL_MOUSEWHEEL events
  void handleSDLEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEWHEEL) {
      mouse_wheel_y += event.wheel.y;
    }
  }

  // Keyboard
  bool isKeyPressed(SDL_Scancode sc) const {
    return current_state[sc];
  }
  bool wasKeyPressed(SDL_Scancode sc) const {
    return last_state[sc];
  }
  bool justPressed(SDL_Scancode sc) const {
    return current_state[sc] && !last_state[sc];
  }
  bool justReleased(SDL_Scancode sc) const {
    return !current_state[sc] && last_state[sc];
  }

  // Mouse
  bool isMouseButtonPressed(Uint8 button) const {
    return mouse_buttons & SDL_BUTTON(button);
  }
  bool wasMouseButtonPressed(Uint8 button) const {
    return last_mouse_buttons & SDL_BUTTON(button);
  }
  bool mouseButtonJustPressed(Uint8 button) const {
    return (mouse_buttons & SDL_BUTTON(button)) && !(last_mouse_buttons & SDL_BUTTON(button));
  }
  bool mouseButtonJustReleased(Uint8 button) const {
    return !(mouse_buttons & SDL_BUTTON(button)) && (last_mouse_buttons & SDL_BUTTON(button));
  }
  int getMouseX() const { return mouse_x; }
  int getMouseY() const { return mouse_y; }
  int getMouseDeltaX() const { return mouse_x - last_mouse_x; }
  int getMouseDeltaY() const { return mouse_y - last_mouse_y; }
  int getMouseWheelY() const { return mouse_wheel_y; }
};

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

  Matrix *matrix = new Matrix();

  SimulationTexture *simulation_texture = new SimulationTexture();
  simulation_texture->initializeTexture(Renderer::getRenderer());

  //-------------------------------------------
  // Simulation State Variables
  //-------------------------------------------
  Input input;
  SDL_Event event;
  bool running = true;

  uint8_t brush_size = 3;
  int simulation_mouse_x;
  int simulation_mouse_y;

  bool show_stats = false;
  bool debug_mode = false;
  
  float lag = 0;

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
    // Input
    //-------------------------------------------
    input.updateState();

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
      input.handleSDLEvent(event);
    }

    if (input.justPressed(SDL_SCANCODE_F1)) {
      show_stats = !show_stats;
    }
    if (input.justPressed(SDL_SCANCODE_F2)) {
      simulation_texture->toggleShowChunks();
    }
    if (input.justPressed(SDL_SCANCODE_F3)) {
      debug_mode = !debug_mode;
      matrix->toggleDebugMode();
    }

    // For simulation/brush, still use simulation coordinates
    simulation_mouse_x = input.mouse_x * Simulation::WIDTH / Window::WIDTH;
    simulation_mouse_y = input.mouse_y * Simulation::HEIGHT / Window::HEIGHT;   

    if (input.getMouseWheelY() > 0) {
      brush_size = std::min(brush_size + 1, 50);
    }
    else if (input.getMouseWheelY() < 0) {
      brush_size = std::max(brush_size - 1, 1);
    }   
      
    if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
      matrix->placeElementsInArea(SAND, simulation_mouse_x, simulation_mouse_y, brush_size);
    }
    if (input.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
      matrix->placeElementsInArea(EMPTY, simulation_mouse_x, simulation_mouse_y, brush_size);
    }
    if (input.mouseButtonJustPressed(SDL_BUTTON_MIDDLE)) {
      printElementDebugInfo(matrix, simulation_mouse_x, simulation_mouse_y);
    }

    //-------------------------------------------
    // Update
    //-------------------------------------------
    while (lag >= Simulation::MS_PER_UPDATE) {
      if (debug_mode) {
        if (input.isKeyPressed(SDL_SCANCODE_H)) {
          matrix->updateCellByCell();
        }
        else if (input.isKeyPressed(SDL_SCANCODE_J)) {
          matrix->updateCellByCell(Simulation::WIDTH);
        }
        else if (input.isKeyPressed(SDL_SCANCODE_K)) {
          matrix->updateCellByCell(Simulation::WIDTH * Simulation::HEIGHT);
        }
      }
      if (!debug_mode) matrix->update();
      lag -= Simulation::MS_PER_UPDATE;

      // Increment tick count for stats
      SystemStatsDisplay::incrementTickCount();

      //-------------------------------------------
      // Render
      //-------------------------------------------
      Renderer::renderScene(*matrix, *simulation_texture, simulation_mouse_x, simulation_mouse_y, brush_size);
      Renderer::present();
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
            << "   Health: " << (health ? std::to_string(health->health) + "/" + std::to_string(health->max) : "none")
            << "\n   Temperature: " << (temperature ? std::to_string(temperature->temperature)
            +  " (Threshold: " + std::to_string(temperature->threshold) + ")" : "none")
            << "\n";
}
