// Input.cpp
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
// *Explain this file*
//
// This file implements the Input class, which provides a static interface for keyboard and mouse input handling
// in SDL-based applications. It manages the state of all keys and mouse buttons, supports per-key/button repeat logic
// with configurable thresholds and repeat rates, and provides utility functions for mouse position and scroll wheel tracking.
// The implementation ensures thread-safe access to repeat state and is designed to be updated once per frame.
// This abstraction simplifies input handling for games and interactive simulations.

#include "falling-sand-sim/core/Input.hpp"
#include <SDL.h>
#include <map>
#include <string>
#include <iostream>

// Static scancode map definition: maps string names to SDL_Scancode values
std::unordered_map<std::string, SDL_Scancode> Input::s_scancode_map = {
  {"A", SDL_SCANCODE_A},
  {"B", SDL_SCANCODE_B},
  {"C", SDL_SCANCODE_C},
  {"D", SDL_SCANCODE_D},
  {"E", SDL_SCANCODE_E},
  {"F", SDL_SCANCODE_F},
  {"G", SDL_SCANCODE_G},
  {"H", SDL_SCANCODE_H},
  {"I", SDL_SCANCODE_I},
  {"J", SDL_SCANCODE_J},
  {"K", SDL_SCANCODE_K},
  {"L", SDL_SCANCODE_L},
  {"M", SDL_SCANCODE_M},
  {"N", SDL_SCANCODE_N},
  {"O", SDL_SCANCODE_O},
  {"P", SDL_SCANCODE_P},
  {"Q", SDL_SCANCODE_Q},
  {"R", SDL_SCANCODE_R},
  {"S", SDL_SCANCODE_S},
  {"T", SDL_SCANCODE_T},
  {"U", SDL_SCANCODE_U},
  {"V", SDL_SCANCODE_V},
  {"W", SDL_SCANCODE_W},
  {"X", SDL_SCANCODE_X},
  {"Y", SDL_SCANCODE_Y},
  {"Z", SDL_SCANCODE_Z},
  {"0", SDL_SCANCODE_0},
  {"1", SDL_SCANCODE_1},
  {"2", SDL_SCANCODE_2},
  {"3", SDL_SCANCODE_3},
  {"4", SDL_SCANCODE_4},
  {"5", SDL_SCANCODE_5},
  {"6", SDL_SCANCODE_6},
  {"7", SDL_SCANCODE_7},
  {"8", SDL_SCANCODE_8},
  {"9", SDL_SCANCODE_9},
  {"SPACE", SDL_SCANCODE_SPACE},
  {"RETURN", SDL_SCANCODE_RETURN},
  {"ESCAPE", SDL_SCANCODE_ESCAPE},
  {"TAB", SDL_SCANCODE_TAB},
  {"BACKSPACE", SDL_SCANCODE_BACKSPACE},
  {"UP_ARROW", SDL_SCANCODE_UP},
  {"DOWN_ARROW", SDL_SCANCODE_DOWN},
  {"LEFT_ARROW", SDL_SCANCODE_LEFT},
  {"RIGHT_ARROW", SDL_SCANCODE_RIGHT},
  {"LSHIFT", SDL_SCANCODE_LSHIFT},
  {"RSHIFT", SDL_SCANCODE_RSHIFT},
  {"LCTRL", SDL_SCANCODE_LCTRL},
  {"RCTRL", SDL_SCANCODE_RCTRL},
  {"LALT", SDL_SCANCODE_LALT},
  {"RALT", SDL_SCANCODE_RALT},
  {"F1", SDL_SCANCODE_F1},
  {"F2", SDL_SCANCODE_F2},
  {"F3", SDL_SCANCODE_F3},
  {"F4", SDL_SCANCODE_F4},
  {"F5", SDL_SCANCODE_F5},
  {"F6", SDL_SCANCODE_F6},
  {"F7", SDL_SCANCODE_F7},
  {"F8", SDL_SCANCODE_F8},
  {"F9", SDL_SCANCODE_F9},
  {"F10", SDL_SCANCODE_F10},
  {"F11", SDL_SCANCODE_F11},
  {"F12", SDL_SCANCODE_F12},
};

// Static members for thread-safe state tracking
std::unordered_map<InputRepeatParams, InputRepeatState> Input::s_input_repeat_map; // Stores state for each key+params
std::mutex Input::s_input_repeat_mutex; // Guards access to s_input_repeat_map

// Keyboard state
const Uint8* Input::s_sdl_state;
std::vector<Uint8> Input::s_current_state;
std::vector<Uint8> Input::s_last_state;
int Input::s_num_keys;

// Mouse state
Uint32 Input::s_mouse_buttons;
Uint32 Input::s_last_mouse_buttons;
int Input::s_mouse_x, Input::s_mouse_y;
int Input::s_last_mouse_x, Input::s_last_mouse_y;
int Input::s_mouse_wheel_y, Input::s_last_mouse_wheel_y;

// Add a static flag to ensure init() is only called once
static bool s_initialized = false;

/**
 * @brief Initialize static input state. Must be called once after SDL is initialized.
 */
void Input::initialize() {
  if (s_initialized) return;
  s_initialized = true;

  s_sdl_state = SDL_GetKeyboardState(&s_num_keys);
  s_current_state.resize(s_num_keys);
  s_last_state.resize(s_num_keys);
  memcpy(s_current_state.data(), s_sdl_state, s_num_keys);
  memcpy(s_last_state.data(), s_sdl_state, s_num_keys);

  s_mouse_buttons = SDL_GetMouseState(&s_mouse_x, &s_mouse_y);
  s_last_mouse_buttons = s_mouse_buttons;
  s_last_mouse_x = s_mouse_x;
  s_last_mouse_y = s_mouse_y;
  s_mouse_wheel_y = 0;
  s_last_mouse_wheel_y = 0;
}

/**
 * @brief Updates the internal state of all keys and mouse buttons.
 * Call this once per frame before querying input states.
 */
void Input::updateState() {
  s_last_state = s_current_state;
  memcpy(s_current_state.data(), SDL_GetKeyboardState(NULL), s_num_keys);

  s_last_mouse_buttons = s_mouse_buttons;
  s_last_mouse_x = s_mouse_x;
  s_last_mouse_y = s_mouse_y;
  s_last_mouse_wheel_y = s_mouse_wheel_y;

  s_mouse_buttons = SDL_GetMouseState(&s_mouse_x, &s_mouse_y);
  s_mouse_wheel_y = 0; // Reset, will be set by handleSDLEvent
}

/**
 * @brief Handles SDL events (specifically SDL_MOUSEWHEEL).
 * Call this from your SDL event loop for SDL_MOUSEWHEEL events.
 * @param event SDL_Event reference
 */
void Input::handleSDLEvent(const SDL_Event& event) {
  if (event.type == SDL_MOUSEWHEEL) {
    s_mouse_wheel_y += event.wheel.y;
  }
}

/**
 * @brief Query if a key is "pressed" with optional repeat/threshold logic.
 * Thread-safe.
 * @param input Key name (e.g., "A", "SPACE")
 * @param threshold Frames to wait before first repeat (default 0)
 * @param repeat_rate Frames between repeats after threshold (default 0)
 * @return true if key is considered pressed this frame
 */
bool Input::getKey(const std::string& input, int threshold, int repeat_rate) {
  // Look up the SDL scancode for the given key string
  auto it = s_scancode_map.find(input);
  if (it == s_scancode_map.end()) {
    std::cerr << "Error! " << input << " is an invalid key input\n";
    return false;
  }
  SDL_Scancode code = it->second;

  // If the key is not currently pressed, reset and remove its state
  if (!s_current_state[code]) {
    std::lock_guard<std::mutex> lock(s_input_repeat_mutex);
    InputRepeatParams params{input, threshold, repeat_rate};
    s_input_repeat_map.erase(params); // Remove state so next press starts fresh
    return false;
  }

  // Key is held: update and check repeat/threshold logic
  InputRepeatParams params{input, threshold, repeat_rate};
  std::lock_guard<std::mutex> lock(s_input_repeat_mutex);
  auto& state = s_input_repeat_map[params]; // Get or create state for this key+params

  // On first press, initialize counters
  if (state.threshold_counter == 0 && state.repeat_counter == 0) {
    state.threshold_counter = threshold;
    state.repeat_counter = repeat_rate;
  }

  // Handle threshold countdown (initial delay before first "true")
  if (state.threshold_counter > 0) {
    state.threshold_counter--;
    return state.threshold_counter == 0; // Return true only on the frame threshold reaches 0
  } 
  // After threshold, handle repeat logic
  else if (repeat_rate > 0) {
    state.repeat_counter--;
    if (state.repeat_counter == 0) {
      state.repeat_counter = repeat_rate; // Reset for next repeat
      return true; // Return true every repeat_rate frames
    }
    return false; // Not time for repeat yet
  } 
  else {
    // If repeat_rate is 0, just return true every frame after threshold
    return true;
  }
}

/**
 * @brief Query if a key was pressed down this frame (no repeat).
 * @param input Key name
 * @return true if key was pressed down this frame
 */
bool Input::getKeyDown(const std::string& input) {
  // Look up the SDL scancode for the given key string
  auto it = s_scancode_map.find(input);
  if (it == s_scancode_map.end()) {
    std::cerr << "Error! " << input << " is an invalid key input\n";
    return false;
  }
  SDL_Scancode code = it->second; 

  return s_current_state[code] && !s_last_state[code];
}

/**
 * @brief Query if a key was released this frame.
 * @param input Key name
 * @return true if key was released this frame
 */
bool Input::getKeyUp(const std::string& input) {
  // Look up the SDL scancode for the given key string
  auto it = s_scancode_map.find(input);
  if (it == s_scancode_map.end()) {
    std::cerr << "Error! " << input << " is an invalid key input\n";
    return false;
  }
  SDL_Scancode code = it->second; 

  return !s_current_state[code] && s_last_state[code]; 
}

/**
 * @brief Query if a mouse button is "pressed" with optional repeat/threshold logic.
 * Thread-safe.
 * @param input Mouse button name ("Left", "Right", "Middle")
 * @param threshold Frames to wait before first repeat (default 0)
 * @param repeat_rate Frames between repeats after threshold (default 0)
 * @return true if button is considered pressed this frame
 */
bool Input::getMouse(const std::string& input, int threshold, int repeat_rate) {
  // Map input string to SDL button mask
  Uint32 button_mask = 0;
  if (input == "LEFT") button_mask = SDL_BUTTON(SDL_BUTTON_LEFT);
  else if (input == "RIGHT") button_mask = SDL_BUTTON(SDL_BUTTON_RIGHT);
  else if (input == "MIDDLE") button_mask = SDL_BUTTON(SDL_BUTTON_MIDDLE);
  else {
    std::cerr << "Error! " << input << " is an invalid mouse input\n";
    return false;
  }

  // If button is not pressed, reset state
  if (!(s_mouse_buttons & button_mask)) {
    std::lock_guard<std::mutex> lock(s_input_repeat_mutex);
    InputRepeatParams params{input, threshold, repeat_rate};
    s_input_repeat_map.erase(params);
    return false;
  }

  // Button is held: update and check repeat/threshold logic
  InputRepeatParams params{input, threshold, repeat_rate};
  std::lock_guard<std::mutex> lock(s_input_repeat_mutex);
  auto& state = s_input_repeat_map[params];

  // On first press, initialize counters
  if (state.threshold_counter == 0 && state.repeat_counter == 0) {
    state.threshold_counter = threshold;
    state.repeat_counter = repeat_rate;
  }

  // Handle threshold countdown (initial delay before first "true")
  if (state.threshold_counter > 0) {
    state.threshold_counter--;
    return state.threshold_counter == 0;
  }
  // After threshold, handle repeat logic
  else if (repeat_rate > 0) {
    state.repeat_counter--;
    if (state.repeat_counter == 0) {
      state.repeat_counter = repeat_rate;
      return true;
    }
    return false;
  } else {
    // If repeat_rate is 0, just return true every frame after threshold
    return true;
  }
}

/**
 * @brief Query if a mouse button was pressed down this frame (no repeat).
 * @param input Mouse button name
 * @return true if button was pressed down this frame
 */
bool Input::getMouseDown(const std::string& input) {
  if (input == "LEFT") {
    return (s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) && !(s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
  } else if (input == "RIGHT") {
    return (s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && !(s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));
  } else if (input == "MIDDLE") {
    return (s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) && !(s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE));
  } else {
    std::cerr << "Error! " << input << " is an invalid mouse input\n";
    return false;
  }
}

/**
 * @brief Query if a mouse button was released this frame.
 * @param input Mouse button name
 * @return true if button was released this frame
 */
bool Input::getMouseUp(const std::string& input) {
  if (input == "LEFT") {
    return (s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) && !(s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
  } else if (input == "RIGHT") {
    return (s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && !(s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));
  } else if (input == "MIDDLE") {
    return (s_last_mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) && !(s_mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE));
  } else {
    std::cerr << "Error! " << input << " is an invalid mouse input\n";
    return false;
  } 
}

/**
 * @brief Get current mouse X position.
 * @return Mouse X coordinate
 */
int Input::getMouseX() {
  return s_mouse_x;
}

/**
 * @brief Get current mouse Y position.
 * @return Mouse Y coordinate
 */
int Input::getMouseY() {
  return s_mouse_y;  
}

/**
 * @brief Get mouse X movement delta since last frame.
 * @return Delta X
 */
int Input::getMouseDeltaX() {
  return s_mouse_x - s_last_mouse_x;  
}

/**
 * @brief Get mouse Y movement delta since last frame.
 * @return Delta Y
 */
int Input::getMouseDeltaY() {
  return s_mouse_y - s_last_mouse_y;  
}

/**
 * @brief Get scroll wheel delta since last frame.
 * @return Scroll delta (positive = up, negative = down)
 */
int Input::getScrollDelta() {
  return s_mouse_wheel_y;  
}

