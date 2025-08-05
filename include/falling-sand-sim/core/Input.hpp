// Input.hpp
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
// This file defines the Input class, which provides a static interface for handling keyboard and mouse input
// in an SDL-based application. It supports querying the current, pressed, and released state of keys and mouse buttons,
// as well as advanced features like configurable key/button repeat with customizable thresholds and repeat rates.
// The class maintains internal state for all keys and mouse buttons, supports thread-safe access to repeat logic,
// and provides utility functions for mouse position and scroll wheel tracking. The Input class is designed to be
// updated once per frame (via updateState), and can be integrated with the SDL event loop for scroll wheel events.
// This abstraction allows for flexible and robust input handling in games or interactive simulations.

#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>

/**
 * @brief State for each unique (key/button, threshold, repeat_rate) combination.
 * Used internally for managing repeat logic.
 */
struct InputRepeatState {
  int threshold_counter; // Counts down from threshold to 0 on initial hold
  int repeat_counter;    // Counts down from repeat_rate to 0 after threshold
};

/**
 * @brief Used as a key in the unordered_map to uniquely identify each key/button+params combo.
 */
struct InputRepeatParams {
  std::string input;   // Key or mouse button name, e.g., "A" or "Left"
  int threshold;       // Initial delay before first "press" is reported
  int repeat_rate;     // How many frames between repeats after threshold

  // Needed for unordered_map key comparison
  bool operator==(const InputRepeatParams& other) const {
    return input == other.input && threshold == other.threshold && repeat_rate == other.repeat_rate;
  }
};

// Hash function for InputRepeatParams so it can be used in unordered_map
namespace std {
  template <>
  struct hash<InputRepeatParams> {
    std::size_t operator()(const InputRepeatParams& k) const {
      // Combine hashes of all fields for uniqueness
      return ((std::hash<std::string>()(k.input)
              ^ (std::hash<int>()(k.threshold) << 1)) >> 1)
              ^ (std::hash<int>()(k.repeat_rate) << 1);
    }
  };
} // namespace std

/**
 * @brief Static class for handling keyboard and mouse input with repeat logic.
 *
 * Usage:
 *   - Call Input::updateState() once per frame to update input states.
 *   - Use Input::handleSDLEvent() from your SDL event loop for SDL_MOUSEWHEEL events.
 *   - Query input states using getKey, getKeyDown, getKeyUp, getMouse, getMouseDown, getMouseUp.
 *   - Use getMouseX/Y, getMouseDeltaX/Y, getScrollDelta for mouse position and scroll.
 */
class Input {
public:
  /**
   * @brief Initialize static input state. Must be called once after SDL is initialized.
   */
  static void initialize();

  /**
   * @brief Update the internal state of all keys and mouse buttons.
   * Call this once per frame, before querying input states.
   */
  static void updateState();

  /**
   * @brief Handle SDL events (specifically SDL_MOUSEWHEEL).
   * Call this from your SDL event loop for SDL_MOUSEWHEEL events.
   * @param event SDL_Event reference
   */
  static void handleSDLEvent(const SDL_Event& event);
  
  /**
   * @brief Query if a key is "pressed" with optional repeat/threshold logic.
   * @param input Key name (e.g., "A", "SPACE")
   * @param threshold Frames to wait before first repeat (default 0)
   * @param repeat_rate Frames between repeats after threshold (default 0)
   * @return true if key is considered pressed this frame
   */
  static bool getKey(const std::string& input, int threshold = 0, int repeat_rate = 0);

  /**
   * @brief Query if a key was pressed down this frame (no repeat).
   * @param input Key name
   * @return true if key was pressed down this frame
   */
  static bool getKeyDown(const std::string& input);

  /**
   * @brief Query if a key was released this frame.
   * @param input Key name
   * @return true if key was released this frame
   */
  static bool getKeyUp(const std::string& input);

  /**
   * @brief Query if a mouse button is "pressed" with optional repeat/threshold logic.
   * @param input Mouse button name ("Left", "Right", "Middle")
   * @param threshold Frames to wait before first repeat (default 0)
   * @param repeat_rate Frames between repeats after threshold (default 0)
   * @return true if button is considered pressed this frame
   */
  static bool getMouse(const std::string& input, int threshold = 0, int repeat_rate = 0);

  /**
   * @brief Query if a mouse button was pressed down this frame (no repeat).
   * @param input Mouse button name
   * @return true if button was pressed down this frame
   */
  static bool getMouseDown(const std::string& input);

  /**
   * @brief Query if a mouse button was released this frame.
   * @param input Mouse button name
   * @return true if button was released this frame
   */
  static bool getMouseUp(const std::string& input);

  /**
   * @brief Get current mouse X position.
   * @return Mouse X coordinate
   */
  static int getMouseX();

  /**
   * @brief Get current mouse Y position.
   * @return Mouse Y coordinate
   */
  static int getMouseY();

  /**
   * @brief Get mouse X movement delta since last frame.
   * @return Delta X
   */
  static int getMouseDeltaX();

  /**
   * @brief Get mouse Y movement delta since last frame.
   * @return Delta Y
   */
  static int getMouseDeltaY();

  /**
   * @brief Get scroll wheel delta since last frame.
   * @return Scroll delta (positive = up, negative = down)
   */
  static int getScrollDelta();

private:
  // Maps string key names to SDL_Scancode values
  static std::unordered_map<std::string, SDL_Scancode> s_scancode_map; 

  // Stores state for each key/button+params for repeat logic (thread-safe)
  static std::unordered_map<InputRepeatParams, InputRepeatState> s_input_repeat_map;
  static std::mutex s_input_repeat_mutex; // Guards access to s_input_repeat_map

  // Keyboard state
  const static Uint8* s_sdl_state;      // Raw SDL keyboard state pointer
  static std::vector<Uint8> s_current_state; // Current frame keyboard state
  static std::vector<Uint8> s_last_state;    // Previous frame keyboard state
  static int s_num_keys;                // Number of keys tracked

  // Mouse state
  static Uint32 s_mouse_buttons;        // Current mouse button bitmask
  static Uint32 s_last_mouse_buttons;   // Previous mouse button bitmask
  static int s_mouse_x, s_mouse_y;      // Current mouse position
  static int s_last_mouse_x, s_last_mouse_y; // Previous mouse position
  static int s_mouse_wheel_y, s_last_mouse_wheel_y; // Scroll wheel state

};

#endif // INPUT_HPP