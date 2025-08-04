// SystemStatsDisplay.hpp
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
// Declares the SystemStatsDisplay class for rendering FPS, TPS, memory, and
// entity/component stats overlay in the simulation.

#ifndef SYSTEM_STATS_DISPLAY
#define SYSTEM_STATS_DISPLAY

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * @brief UI overlay for displaying debug information (FPS, chunk stats, etc.).
 * 
 * Handles text rendering and updating debug stats.
 */
class SystemStatsDisplay {
public:
	/**
	 * @brief Initialize the debug UI (load font).
	 * @return true on success, false on failure
	 */
	static bool initialize();

	/**
	 * @brief Update debug stats (call once per frame).
	 * @param currentTime Current SDL ticks
	 * @param activeChunks Number of active chunks
	 * @param totalChunks Total number of chunks
	 */
	static void update(const Uint32 currentTime, const bool isActive);

	/**
	 * @brief Render the debug overlay (call after all other rendering).
	 * @param debugEnabled Whether to render the overlay
	 */
	static void render();

  static void incrementFrameCount();
  static void incrementTickCount();

	// UI layout constants
	static constexpr int FONT_SIZE = 16;      ///< Font size for debug text
	static constexpr int TEXT_POS_X = 10;     ///< X position for debug text
	static constexpr int TEXT_POS_Y = 10;     ///< Y position for debug text

private:
	/**
	 * @brief Rebuild the debug text texture from the given string.
	 * @param text The text to render
	 */
	static void rebuildTextTexture(const std::string& text);

	static TTF_Font* sp_font;   					///< Loaded font
	static SDL_Texture* sp_text_texture;  ///< Texture for rendered debug text
	static SDL_Rect s_destination_rect;   ///< Destination rect for text
	static Uint32 s_fps_last_time;     	  ///< Last time FPS was calculated
	static int s_frame_count;         		///< Frame count since last FPS update
	static float s_fps;            			  ///< Calculated FPS
	static bool s_is_active;
  static Uint32 s_tick_last_time;
  static int s_tick_count;
  static float s_tick_rate;
};

#endif // SYSTEM_STATS_DISPLAY