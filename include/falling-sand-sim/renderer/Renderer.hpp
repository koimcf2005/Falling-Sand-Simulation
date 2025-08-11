// Renderer.hpp
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
// Declares the Renderer class for SDL window, rendering, and drawing
// utilities for the Falling Sand Simulation.

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "falling-sand-sim/world/Matrix.hpp"
#include "falling-sand-sim/core/Globals.hpp"
#include "falling-sand-sim/renderer/Viewport.hpp"

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
	 * @brief Clear the screen to black.#include "falling-sand-sim/renderer/SimulationTexture.hpp"

	 */
	static void clear();

	/**
	 * @brief Present the rendered frame to the window.
	 */
	static void present();
	/**
   * 
	 * @brief Cleanup and destroy all SDL resources and UI overlays.
	 */
	static void cleanup();

	/**
	 * @brief Get the underlying SDL_Renderer pointer.
	 */
	static SDL_Renderer* getRenderer();

	/**
	 * @brief Set SDL logical rendering size to simulation grid size.
	 */
	static void setSimulationResolution();

	/**
	 * @brief Reset SDL logical rendering size to window size (full resolution).
	 */
	static void setWindowResolution();

	/**
   * @brief Sets the SDL logical rendering size to the giveb viewport size.
	 */
  static void setViewportResolution();

	/**
	 * @brief Render the simulation scene, UI, and debug overlays.
	 * @param matrix CellularMatrix to render
	 * @param showDebug Whether to show the debug overlay
	 */
	static void renderMatrixThroughViewport(Matrix& matrix);

  static void queueRectangleToWindow(
    const int x, const int y,
    const int width, const int height,
    const int thickness,
    const SDL_Color& color
  );  

  static void queueRectangleToViewport(
    const int x, const int y,
    const int width, const int height,
    const int thickness,
    const SDL_Color& color
  );

  static void queueCircleToWindow(
    const int x, const int y,
    const int radius,
    const SDL_Color& color
  );

  static void queueCircleToViewport(
    const int x, const int y,
    const int radius,
    const SDL_Color& color
  );

  // Converts window (screen) coordinates to viewport (texture) coordinates
  static std::pair<int, int> windowToViewportCoords(int winX, int winY);

  // Converts viewport (texture) coordinates to simulation coordinates
  static std::pair<int, int> viewportToSimulationCoords(int vpX, int vpY);

  // Converts simulation coordinates to viewport (texture) coordinates
  static std::pair<int, int> simulationToViewportCoords(int simX, int simY);

  // Converts viewport (texture) coordinates to window (screen) coordinates
  static std::pair<int, int> viewportToWindowCoords(int vpX, int vpY);

  // Converts window (screen) coordinates to simulation coordinates (convenience)
  static std::pair<int, int> windowToSimulationCoords(int winX, int winY);

  // Converts simulation coordinates to window (screen) coordinates (convenience)
  static std::pair<int, int> simulationToWindowCoords(int simX, int simY);

private:
	// SDL window and renderer
	static SDL_Window* sp_window;
	static SDL_Renderer* sp_renderer;

	// Struct for queued screen-space rectangles
	struct QueuedRectangle {
		int x, y;
    int width, height;
    int thickness;
		SDL_Color color;
	};

	// Struct for queued brush outlines
	struct QueuedCircle {
		int x, y;
		int radius;
		SDL_Color color;
	};

	static std::vector<QueuedRectangle> s_queued_window_rectangles;
  static std::vector<QueuedRectangle> s_queued_viewport_rectangles;
	static std::vector<QueuedCircle> s_queued_window_circles;
  static std::vector<QueuedCircle> s_queued_viewport_circles;

	/**
	 * @brief Draw all queued screen-space rectangles (used for overlays).
	 */
	static void drawQueuedRectangles();
  static void drawRectangle(const QueuedRectangle& rect);
	/**
	 * @brief Draw all queued brush outlines (used for overlays).
	 */
	static void drawQueuedCircles();
  static void drawCircle(const QueuedCircle& circle);

  enum ResolutionType {
    WINDOW,
    SIMULATION,
    VIEWPORT
  };
  static ResolutionType s_current_resolution;  
};

#endif // RENDERER_HPP