// src/core/Renderer.hpp
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include "src/core/CellularMatrix.hpp"
#include "src/core/Globals.hpp"
#include "src/ui/ElementUI.hpp"
#include "src/ui/DebugUI.hpp"

/**
 * @brief Handles all rendering and UI overlay for the simulation.
 * 
 * Manages SDL window, renderer, logical resolution, UI overlays, and debug overlays.
 * Provides utility functions for drawing textures, brush outlines, and screen-space rectangles.
 */
class Renderer {
public:
	/**
	 * @brief Construct a new Renderer object. Does not initialize SDL or create window/renderer.
	 */
	Renderer();

	/**
	 * @brief Initialize SDL, create window and renderer, and initialize UI overlays.
	 * @param title Window title
	 * @return true on success, false on failure
	 */
	bool initialize(const char* title);

	/**
	 * @brief Clear the screen to black.
	 */
	void clear();

	/**
	 * @brief Present the rendered frame to the window.
	 */
	void present();

	/**
	 * @brief Set SDL logical rendering size to simulation grid size.
	 */
	void setLogicalResolution();

	/**
	 * @brief Reset SDL logical rendering size to window size (full resolution).
	 */
	void resetLogicalResolution();

	/**
	 * @brief Draw a texture to the renderer.
	 * @param texture SDL_Texture to draw
	 */
	void drawTexture(SDL_Texture* texture);

	/**
	 * @brief Draw a circular outline for the brush at the given position.
	 * @param mouseX X position in simulation coordinates
	 * @param mouseY Y position in simulation coordinates
	 * @param radius Brush radius
	 * @param mouseOverUI If true, do not draw the outline
	 */
	void drawBrushOutline(int mouseX, int mouseY, int radius, bool mouseOverUI);

	/**
	 * @brief Draw a rectangle in screen (window) space with a given thickness.
	 * @param x Top-left X in simulation coordinates
	 * @param y Top-left Y in simulation coordinates
	 * @param width Width in simulation coordinates
	 * @param height Height in simulation coordinates
	 * @param thickness Border thickness in pixels
	 */
	void drawScreenSpaceRect(int x, int y, int width, int height, int thickness);

	/**
	 * @brief Render the simulation scene, UI, and debug overlays.
	 * @param matrix CellularMatrix to render
	 * @param showDebug Whether to show the debug overlay
	 */
	void renderScene(CellularMatrix& matrix, bool showDebug);

	/**
	 * @brief Get the underlying SDL_Renderer pointer.
	 */
	SDL_Renderer* getRenderer();

	/**
	 * @brief Get the ElementUI overlay.
	 */
	ElementUI* getElementUI();

	/**
	 * @brief Get the DebugUI overlay.
	 */
	DebugUI* getDebugUI();

	/**
	 * @brief Cleanup and destroy all SDL resources and UI overlays.
	 */
	void cleanup();

	/**
	 * @brief Convert window (screen) coordinates to simulation (render) coordinates.
	 * @param winX Window X
	 * @param winY Window Y
	 * @return Pair of (renderX, renderY)
	 */
	std::pair<int, int> windowToRenderCoords(int winX, int winY) const;

	/**
	 * @brief Convert simulation (render) coordinates to window (screen) coordinates.
	 * @param renderX Simulation X
	 * @param renderY Simulation Y
	 * @return Pair of (winX, winY)
	 */
	std::pair<int, int> renderToWindowCoords(int renderX, int renderY) const;

private:
	// SDL window and renderer
	SDL_Window* mp_Window = nullptr;
	SDL_Renderer* mp_Renderer = nullptr;

	// UI overlays
	ElementUI* mp_ElementUI = nullptr;
	DebugUI* mp_DebugUI = nullptr;

	// Utility font pointer (not used directly in Renderer, but may be used by overlays)
	TTF_Font* mp_Font {nullptr};

	// Struct for queued screen-space rectangles
	struct ScreenRect {
		int x, y, w, h, thickness;
	};
	std::vector<ScreenRect> m_QueuedRects;

	// Lighting system
	SDL_Texture* mp_LightMap = nullptr;
	// Change to store RGB for each cell (3 bytes per cell)
	std::vector<uint8_t> m_LightBuffer; // R, G, B per cell

	struct PointLight {
		int x, y, radius;
		uint8_t intensity, colorR, colorG, colorB;
		float falloffRate;
	};
	std::vector<PointLight> m_PointLights;

	/**
	 * @brief Draw all queued screen-space rectangles (used for overlays).
	 */
	void drawQueuedRects();

	/**
	 * @brief Draw a circle outline using integer coordinates.
	 * @param centerX Center X
	 * @param centerY Center Y
	 * @param radius Radius
	 */
	void drawCircleOutline(int centerX, int centerY, int radius);
};

#endif // RENDERER_HPP