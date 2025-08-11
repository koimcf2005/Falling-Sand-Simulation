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

#include <fmt/core.h>
#include <iostream>

//------------------------------------------------------------------------------
// Static member definitions
//------------------------------------------------------------------------------
SDL_Window* Renderer::sp_window = nullptr;
SDL_Renderer* Renderer::sp_renderer = nullptr;
std::vector<Renderer::QueuedRectangle> Renderer::s_queued_window_rectangles;
std::vector<Renderer::QueuedRectangle> Renderer::s_queued_viewport_rectangles;
std::vector<Renderer::QueuedCircle> Renderer::s_queued_window_circles;
std::vector<Renderer::QueuedCircle> Renderer::s_queued_viewport_circles;
Renderer::ResolutionType Renderer::s_current_resolution = WINDOW;

//------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------
bool Renderer::initialize(const char* title) {

  //------------------------------------------------------------------------------
  // SDL2 Rendering Initialization
  //------------------------------------------------------------------------------

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
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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
  if (!s_current_resolution == SIMULATION) {
    SDL_RenderSetLogicalSize(sp_renderer, Simulation::WIDTH, Simulation::HEIGHT);
    s_current_resolution = SIMULATION; 
  }
}

void Renderer::setWindowResolution() {
	// Reset logical rendering size to window size (full resolution)
  if (!s_current_resolution == WINDOW) {
    SDL_RenderSetLogicalSize(sp_renderer, 0, 0);
    s_current_resolution = WINDOW;
  }
}

void Renderer::setViewportResolution() {
  SDL_RenderSetLogicalSize(sp_renderer, Viewport::getWidth(), Viewport::getHeight());
  s_current_resolution = VIEWPORT;
}

void Renderer::renderMatrixThroughViewport(Matrix& matrix) { 
	// Render the simulation scene, UI, and debug overlays
	clear();

	// Draw low-res game world
	Viewport::updateTexture(matrix);
	
  // Get window and simulation sizes
  int winW, winH;
  SDL_GetRendererOutputSize(sp_renderer, &winW, &winH);

  // Use the actual viewport/simulation size
  int simW = Viewport::getWidth();
  int simH = Viewport::getHeight();

  // Calculate aspect ratios
  float winAspect = static_cast<float>(winW) / winH;
  float simAspect = static_cast<float>(simW) / simH;

  SDL_Rect dst;
  if (winAspect > simAspect) {
      // Window is wider than simulation: pillarbox
      dst.h = winH;
      dst.w = static_cast<int>(simAspect * winH);
      dst.x = (winW - dst.w) / 2;
      dst.y = 0;
  } else {
      // Window is taller than simulation: letterbox
      dst.w = winW;
      dst.h = static_cast<int>(winW / simAspect);
      dst.x = 0;
      dst.y = (winH - dst.h) / 2;
  }

  SDL_RenderCopy(sp_renderer, Viewport::getTexture(), nullptr, &dst);
  
	drawQueuedRectangles();
  
  drawQueuedCircles();

	SystemStatsDisplay::render();
}

void Renderer::queueRectangleToWindow(
  const int x, const int y,
  const int width, const int height,
  const int thickness,
  const SDL_Color& color
) {
  s_queued_window_rectangles.push_back({
    x, y,
    width, height,
    thickness,
    color
  }); 
}  

void Renderer::queueRectangleToViewport(
  const int x, const int y,
  const int width, const int height,
  const int thickness,
  const SDL_Color& color
) {
  s_queued_viewport_rectangles.push_back({
    x, y,
    width, height,
    thickness,
    color
  });
}

void Renderer::queueCircleToWindow(
  const int x, const int y,
  const int radius,
  const SDL_Color& color
) {
  s_queued_window_circles.push_back({
    x, y,
    radius,
    color
  });
}

void Renderer::queueCircleToViewport(
  const int x, const int y,
  const int radius,
  const SDL_Color& color
) {
  s_queued_viewport_circles.push_back({
    x, y,
    radius,
    color
  });
}

void Renderer::drawQueuedRectangles() {
  setViewportResolution();
  for (const auto& rect : s_queued_viewport_rectangles) {
    drawRectangle(rect);  
  }
  setWindowResolution();
  for (const auto& rect : s_queued_window_rectangles) {
    drawRectangle(rect);
  }
  s_queued_window_rectangles.clear();
  s_queued_viewport_rectangles.clear();
}

void Renderer::drawRectangle(const QueuedRectangle& rect) {
  SDL_SetRenderDrawColor(sp_renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
  SDL_Rect top    = { rect.x, rect.y, rect.width, rect.thickness };
  SDL_Rect bottom = { rect.x, rect.y + rect.height - rect.thickness, rect.width, rect.thickness };
  SDL_Rect left   = { rect.x, rect.y, rect.thickness, rect.height };
  SDL_Rect right  = { rect.x + rect.width - rect.thickness, rect.y, rect.thickness, rect.height };

  SDL_RenderFillRect(sp_renderer, &top);
  SDL_RenderFillRect(sp_renderer, &bottom);
  SDL_RenderFillRect(sp_renderer, &left);
  SDL_RenderFillRect(sp_renderer, &right); 
}

void Renderer::drawQueuedCircles() {
  setViewportResolution();
  for (const auto& circle : s_queued_viewport_circles) {
    drawCircle(circle);  
  }
  setWindowResolution();
  for (const auto& circle : s_queued_window_circles) {
    drawCircle(circle);
  }
  s_queued_window_circles.clear();
  s_queued_viewport_circles.clear();
}

void Renderer::drawCircle(const QueuedCircle& circle) {
  SDL_SetRenderDrawColor(sp_renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
  
  const int diameter = circle.radius * 2;
  const int center_x = circle.x;
  const int center_y = circle.y;
  int x = circle.radius - 1, y = 0;
  int tx = 1, ty = 1;
  int err = tx - diameter;

  while (x >= y) {
    SDL_RenderDrawPoint(sp_renderer, center_x + x, center_y - y);
    SDL_RenderDrawPoint(sp_renderer, center_x + x, center_y + y);
    SDL_RenderDrawPoint(sp_renderer, center_x - x, center_y - y);
    SDL_RenderDrawPoint(sp_renderer, center_x - x, center_y + y);
    SDL_RenderDrawPoint(sp_renderer, center_x + y, center_y - x);
    SDL_RenderDrawPoint(sp_renderer, center_x + y, center_y + x);
    SDL_RenderDrawPoint(sp_renderer, center_x - y, center_y - x);
    SDL_RenderDrawPoint(sp_renderer, center_x - y, center_y + x);

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

std::pair<int, int> Renderer::windowToViewportCoords(int winX, int winY) {
    int winW, winH;
    SDL_GetRendererOutputSize(sp_renderer, &winW, &winH);

    int vpW = Viewport::getWidth();
    int vpH = Viewport::getHeight();

    float vpAspect = static_cast<float>(vpW) / vpH;
    float winAspect = static_cast<float>(winW) / winH;

    int dst_x, dst_y, dst_w, dst_h;
    if (winAspect > vpAspect) {
        dst_h = winH;
        dst_w = static_cast<int>(vpAspect * winH);
        dst_x = (winW - dst_w) / 2;
        dst_y = 0;
    } else {
        dst_w = winW;
        dst_h = static_cast<int>(winW / vpAspect);
        dst_x = 0;
        dst_y = (winH - dst_h) / 2;
    }

    // If outside viewport, return -1
    if (winX < dst_x || winX >= dst_x + dst_w || winY < dst_y || winY >= dst_y + dst_h)
        return {-1, -1};

    float norm_x = (winX - dst_x) / static_cast<float>(dst_w);
    float norm_y = (winY - dst_y) / static_cast<float>(dst_h);

    int vpX = static_cast<int>(norm_x * vpW);
    int vpY = static_cast<int>(norm_y * vpH);
    return {vpX, vpY};
}

std::pair<int, int> Renderer::viewportToSimulationCoords(int vpX, int vpY) {
    int simX = Viewport::getPositionX() + vpX;
    int simY = Viewport::getPositionY() + vpY;
    return {simX, simY};
}

std::pair<int, int> Renderer::simulationToViewportCoords(int simX, int simY) {
    int vpX = simX - Viewport::getPositionX();
    int vpY = simY - Viewport::getPositionY();
    return {vpX, vpY};
}

std::pair<int, int> Renderer::viewportToWindowCoords(int vpX, int vpY) {
    int winW, winH;
    SDL_GetRendererOutputSize(sp_renderer, &winW, &winH);

    int vpW = Viewport::getWidth();
    int vpH = Viewport::getHeight();

    float vpAspect = static_cast<float>(vpW) / vpH;
    float winAspect = static_cast<float>(winW) / winH;

    int dst_x, dst_y, dst_w, dst_h;
    if (winAspect > vpAspect) {
        dst_h = winH;
        dst_w = static_cast<int>(vpAspect * winH);
        dst_x = (winW - dst_w) / 2;
        dst_y = 0;
    } else {
        dst_w = winW;
        dst_h = static_cast<int>(winW / vpAspect);
        dst_x = 0;
        dst_y = (winH - dst_h) / 2;
    }

    int winX = dst_x + static_cast<int>((vpX / static_cast<float>(vpW)) * dst_w);
    int winY = dst_y + static_cast<int>((vpY / static_cast<float>(vpH)) * dst_h);
    return {winX, winY};
}

std::pair<int, int> Renderer::windowToSimulationCoords(int winX, int winY) {
    auto [vpX, vpY] = windowToViewportCoords(winX, winY);
    if (vpX == -1 || vpY == -1) return {-1, -1};
    return viewportToSimulationCoords(vpX, vpY);
}

std::pair<int, int> Renderer::simulationToWindowCoords(int simX, int simY) {
    auto [vpX, vpY] = simulationToViewportCoords(simX, simY);
    return viewportToWindowCoords(vpX, vpY);
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