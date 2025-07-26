#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "src/matrix/Matrix.hpp"
#include "src/core/Globals.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <vector>

/**
 * @brief Handles all rendering and UI overlay for the simulation.
 * 
 * Manages SDL window, renderer, logical resolution, UI overlays, and debug overlays.
 * Provides utility functions for drawing textures, brush outlines, and screen-space rectangles.
 */
class Renderer {
public:
	/**
	 * @brief Initialize SDL, create window and renderer, and initialize UI overlays.
	 * @param title Window title
	 * @return true on success, false on failure
	 */
	static bool initialize(const char* title);

	/**
	 * @brief Clear the screen to black.
	 */
	static void clear();

	/**
	 * @brief Present the rendered frame to the window.
	 */
	static void present();

	/**
	 * @brief Set SDL logical rendering size to simulation grid size.
	 */
	static void setSimulationResolution();

	/**
	 * @brief Reset SDL logical rendering size to window size (full resolution).
	 */
	static void setWindowResolution();

	/**
	 * @brief Draw a texture to the renderer.
	 * @param texture SDL_Texture to draw
	 */
	static void drawTexture(SDL_Texture* texture);

	/**
	 * @brief Draw a circular outline for the brush at the given position.
	 * @param mouseX X position in simulation coordinates
	 * @param mouseY Y position in simulation coordinates
	 * @param radius Brush radius
	 * @param mouseOverUI If true, do not draw the outline
	 */
	static void drawBrushOutline(int mouseX, int mouseY, int radius, bool mouseOverUI);

	/**
	 * @brief Draw a rectangle in screen (window) space with a given thickness.
	 * @param x Top-left X in simulation coordinates
	 * @param y Top-left Y in simulation coordinates
	 * @param width Width in simulation coordinates
	 * @param height Height in simulation coordinates
	 * @param thickness Border thickness in pixels
	 */
	static void drawScreenSpaceRect(int x, int y, int width, int height, int thickness);

	/**
	 * @brief Render the simulation scene, UI, and debug overlays.
	 * @param matrix CellularMatrix to render
	 * @param showDebug Whether to show the debug overlay
	 */
	static void renderScene(Matrix& matrix);

	/**
	 * @brief Get the underlying SDL_Renderer pointer.
	 */
	static SDL_Renderer* getRenderer();

	// /**
	//  * @brief Get the ElementUI overlay.
	//  */
	// ElementUI* getElementUI();

	// /**
	//  * @brief Get the DebugUI overlay.
	//  */
	// DebugUI* getDebugUI();

	/**
	 * @brief Cleanup and destroy all SDL resources and UI overlays.
	 */
	static void cleanup();

	/**
	 * @brief Convert window (screen) coordinates to simulation (render) coordinates.
	 * @param winX Window X
	 * @param winY Window Y
	 * @return Pair of (renderX, renderY)
	 */
	static std::pair<int, int> windowToRenderCoords(int winX, int winY);

	/**
	 * @brief Convert simulation (render) coordinates to window (screen) coordinates.
	 * @param renderX Simulation X
	 * @param renderY Simulation Y
	 * @return Pair of (winX, winY)
	 */
	static std::pair<int, int> renderToWindowCoords(int renderX, int renderY);

private:
	/**
	 * @brief Draw all queued screen-space rectangles (used for overlays).
	 */
	static void drawQueuedRects();

	/**
	 * @brief Draw a circle outline using integer coordinates.
	 * @param centerX Center X
	 * @param centerY Center Y
	 * @param radius Radius
	 */
	static void drawCircleOutline(int centerX, int centerY, int radius);

	// SDL window and renderer
	static SDL_Window* sp_window;
	static SDL_Renderer* sp_renderer;

	// Utility font pointer (not used directly in Renderer, but may be used by overlays)
	static TTF_Font* sp_font;

	// Struct for queued screen-space rectangles
	struct ScreenRect {
		int x, y, w, h, thickness;
	};
	static std::vector<ScreenRect> s_queued_rects;
};

#endif // RENDERER_HPP