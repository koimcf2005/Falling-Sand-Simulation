#ifndef DEBUG_UI_HPP
#define DEBUG_UI_HPP


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * @brief UI overlay for displaying debug information (FPS, chunk stats, etc.).
 * 
 * Handles text rendering and updating debug stats.
 */
class DebugUI {
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
	static void update(const Uint32 currentTime, const int activeChunks, const int totalChunks, const bool isActive);

	/**
	 * @brief Render the debug overlay (call after all other rendering).
	 * @param debugEnabled Whether to render the overlay
	 */
	static void render();

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
};

#endif // DEBUG_UI_HPP