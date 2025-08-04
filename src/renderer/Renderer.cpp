// Renderer.cpp
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
// Implements the Renderer class for SDL window, rendering, and drawing
// utilities for the Falling Sand Simulation.

#include "falling-sand-sim/renderer/Renderer.hpp"

#include "falling-sand-sim/renderer/SystemStatsDisplay.hpp"

#include <iostream>

//------------------------------------------------------------------------------
// Static member definitions
//------------------------------------------------------------------------------
SDL_Window* Renderer::sp_window = nullptr;
SDL_Renderer* Renderer::sp_renderer = nullptr;
std::vector<Renderer::ScreenRect> Renderer::s_queued_rects;
bool Renderer::s_is_window_resolution = true;

//------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------
bool Renderer::initialize(const char* title) {

  //------------------------------------------------------------------------------
  // SDL2 Initialization
  //------------------------------------------------------------------------------
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cerr << "SDL init failed: " << SDL_GetError() << '\n';
			return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0) {
			std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
			return false;
	}

	if (TTF_Init() != 0) {
			std::cerr << "TTF init failed: " << TTF_GetError() << '\n';
			return false;
	}

	sp_window = SDL_CreateWindow(
			title, 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			Window::WIDTH, 
			Window::HEIGHT, 
			SDL_WINDOW_SHOWN
	);

	if (!sp_window) {
			std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
			return false;
	}

	sp_renderer = SDL_CreateRenderer(
			sp_window, -1, 
			SDL_RENDERER_ACCELERATED
	);

	if (!sp_renderer) {
			std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
			return false;
	}

  //------------------------------------------------------------------------------
  // Custom Initialization
  //------------------------------------------------------------------------------
	if (!SystemStatsDisplay::initialize()) {
		std::cerr << "Failed to initialize Debug UI\n";
		cleanup();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// Rendering and Presentation
//------------------------------------------------------------------------------

void Renderer::clear() {
	// Clear the screen to black
	SDL_SetRenderDrawColor(sp_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sp_renderer);
}

void Renderer::present() {
	// Present the rendered frame to the window
	SDL_RenderPresent(sp_renderer);
}

void Renderer::setSimulationResolution() {
	// Set logical rendering size to simulation grid size
  if (s_is_window_resolution) {
    SDL_RenderSetLogicalSize(sp_renderer, Simulation::WIDTH, Simulation::HEIGHT);
    s_is_window_resolution = false;
  }
}

void Renderer::setWindowResolution() {
	// Reset logical rendering size to window size (full resolution)
  if (!s_is_window_resolution) {
    SDL_RenderSetLogicalSize(sp_renderer, 0, 0);
    s_is_window_resolution = true;
  }
}

void Renderer::renderScene(
  Matrix& matrix,
  SimulationTexture& simulation_texture,
  int cursor_x, int cursor_y,
  int cursor_radius
) { 
	// Render the simulation scene, UI, and debug overlays
	clear();

  setSimulationResolution();

	// Draw low-res game world
	simulation_texture.updateTexture(matrix);
	drawTexture(simulation_texture.getTexture());
  
  drawBrushOutline(cursor_x, cursor_y, cursor_radius);

	// Switch to full-res and render overlays
	setWindowResolution();  

	SystemStatsDisplay::render();

	drawQueuedRects();
}

void Renderer::drawTexture(SDL_Texture* texture) {
	// Draw a texture to the renderer
	SDL_RenderCopy(sp_renderer, texture, nullptr, nullptr);
}

//------------------------------------------------------------------------------
// Drawing Utilities
//------------------------------------------------------------------------------

void Renderer::drawBrushOutline(int x, int y, uint8_t radius) {
	// Draw a circular outline for the brush if not over UI and within bounds
	if (x < 0 || x >= Simulation::WIDTH || y < 0 || y >= Simulation::HEIGHT) return;
	SDL_SetRenderDrawColor(sp_renderer, 255, 255, 255, 255);
	drawCircleOutline(x, y, radius);
}

void Renderer::drawCircleOutline(int centerX, int centerY, uint8_t radius) {
	// Draw a circle outline using the midpoint circle algorithm
	const int diameter = radius * 2;
	int x = radius - 1, y = 0;
	int tx = 1, ty = 1;
	int err = tx - diameter;

	while (x >= y) {
		SDL_RenderDrawPoint(sp_renderer, centerX + x, centerY - y);
		SDL_RenderDrawPoint(sp_renderer, centerX + x, centerY + y);
		SDL_RenderDrawPoint(sp_renderer, centerX - x, centerY - y);
		SDL_RenderDrawPoint(sp_renderer, centerX - x, centerY + y);
		SDL_RenderDrawPoint(sp_renderer, centerX + y, centerY - x);
		SDL_RenderDrawPoint(sp_renderer, centerX + y, centerY + x);
		SDL_RenderDrawPoint(sp_renderer, centerX - y, centerY - x);
		SDL_RenderDrawPoint(sp_renderer, centerX - y, centerY + x);

		if (err <= 0) {
			y++;
			err += ty;
			ty += 2;
		}
		if (err > 0) {
			x--;
			tx += 2;
			err += tx - diameter;
		}
	}
}

void Renderer::drawScreenSpaceRect(int x, int y, int width, int height, int thickness, SDL_Color color) {
	// Queue a rectangle in screen (window) space for drawing
	auto [winX, winY] = renderToWindowCoords(x, y);
	auto [winX2, winY2] = renderToWindowCoords(x + width, y + height);

	int screenWidth = winX2 - winX;
	int screenHeight = winY2 - winY;

	s_queued_rects.push_back({ winX, winY, screenWidth, screenHeight, thickness, color});
}

void Renderer::drawQueuedRects() {
	// Draw all queued screen-space rectangles (used for overlays)
	setWindowResolution();
	
	for (const auto& rect : s_queued_rects) {
		SDL_SetRenderDrawColor(sp_renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a); // Red outlines
		SDL_Rect top    = { rect.x, rect.y, rect.w, rect.thickness };
		SDL_Rect bottom = { rect.x, rect.y + rect.h - rect.thickness, rect.w, rect.thickness };
		SDL_Rect left   = { rect.x, rect.y, rect.thickness, rect.h };
		SDL_Rect right  = { rect.x + rect.w - rect.thickness, rect.y, rect.thickness, rect.h };

		SDL_RenderFillRect(sp_renderer, &top);
		SDL_RenderFillRect(sp_renderer, &bottom);
		SDL_RenderFillRect(sp_renderer, &left);
		SDL_RenderFillRect(sp_renderer, &right);
	}

	s_queued_rects.clear();
	setSimulationResolution();
}

//------------------------------------------------------------------------------
// Coordinate Conversion
//------------------------------------------------------------------------------
std::pair<int, int> Renderer::windowToRenderCoords(int winX, int winY) {
	// Convert window (screen) coordinates to simulation (render) coordinates
	int renderX = winX * Simulation::WIDTH / Window::WIDTH;
	int renderY = winY * Simulation::HEIGHT / Window::HEIGHT;
	return { renderX, renderY };
}

std::pair<int, int> Renderer::renderToWindowCoords(int renderX, int renderY) {
	// Convert simulation (render) coordinates to window (screen) coordinates
	int winX = renderX * Window::WIDTH / Simulation::WIDTH;
	int winY = renderY * Window::HEIGHT / Simulation::HEIGHT;
	return { winX, winY };
}

//------------------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------------------

SDL_Renderer* Renderer::getRenderer() {
	return sp_renderer;
}

//------------------------------------------------------------------------------
// Cleanup
//------------------------------------------------------------------------------

void Renderer::cleanup() {
	// Destroy SDL resources and UI overlays
	if (sp_renderer) SDL_DestroyRenderer(sp_renderer);
	if (sp_window) SDL_DestroyWindow(sp_window);
	IMG_Quit();
  TTF_Quit();
	SDL_Quit();
}