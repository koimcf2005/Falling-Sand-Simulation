// src/ui/ElementUI.hpp
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
	 * @brief Construct a new DebugUI object.
	 * @param renderer SDL_Renderer pointer for rendering
	 */
	DebugUI(SDL_Renderer* renderer);

	/**
	 * @brief Destructor. Cleans up textures and font.
	 */
	~DebugUI();

	/**
	 * @brief Initialize the debug UI (load font).
	 * @return true on success, false on failure
	 */
	bool initialize();

	/**
	 * @brief Update debug stats (call once per frame).
	 * @param currentTime Current SDL ticks
	 * @param activeChunks Number of active chunks
	 * @param totalChunks Total number of chunks
	 */
	void update(Uint32 currentTime, int activeChunks, int totalChunks);

	/**
	 * @brief Render the debug overlay (call after all other rendering).
	 * @param debugEnabled Whether to render the overlay
	 */
	void render(bool debugEnabled);

	// UI layout constants
	static constexpr int FONT_SIZE = 16;      ///< Font size for debug text
	static constexpr int TEXT_POS_X = 10;     ///< X position for debug text
	static constexpr int TEXT_POS_Y = 10;     ///< Y position for debug text

private:
	/**
	 * @brief Rebuild the debug text texture from the given string.
	 * @param text The text to render
	 */
	void rebuildTextTexture(const std::string& text);

	SDL_Renderer* mp_Renderer;    ///< SDL renderer pointer
	TTF_Font* mp_Font {nullptr};   ///< Loaded font
	SDL_Texture* mp_TextTexture {nullptr}; ///< Texture for rendered debug text
	SDL_Rect m_DstRect {0, 0, 0, 0};     ///< Destination rect for text

	Uint32 m_FpsLastTime {0};     ///< Last time FPS was calculated
	int m_FrameCount {0};         ///< Frame count since last FPS update
	float m_Fps {0.f};            ///< Calculated FPS
};

#endif // DEBUG_UI_HPP