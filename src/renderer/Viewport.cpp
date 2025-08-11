// Viewport.cpp
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
// Implements the Viewport class for managing the simulation's display
// texture and chunk overlays.

#include "falling-sand-sim/renderer/Viewport.hpp"

#include "falling-sand-sim/core/Globals.hpp"
#include "falling-sand-sim/renderer/Renderer.hpp"

SDL_Texture* Viewport::s_simulation_texture;

int Viewport::s_zoom {20};

int Viewport::s_width = s_zoom * 16; //320;
int Viewport::s_height = s_zoom * 9; //180;

int Viewport::s_position_x {0};
int Viewport::s_position_y {332};

SDL_Texture* Viewport::getTexture() { return s_simulation_texture; }

/**
 * @brief Initializes the simulation texture for rendering.
 * @param renderer Pointer to the SDL_Renderer.
 */
void Viewport::initialize(SDL_Renderer* renderer) {
  // Clean up existing texture if any
  if (s_simulation_texture) {
    SDL_DestroyTexture(s_simulation_texture);
  }

  // Create streaming texture for efficient updates
  s_simulation_texture = SDL_CreateTexture(
    renderer, 
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING,
    s_width,
    s_height 
  );
  SDL_SetTextureBlendMode(s_simulation_texture, SDL_BLENDMODE_BLEND);
}

/**
 * @brief Updates the simulation texture with the current element states.
 */
void Viewport::updateTexture(const Matrix& matrix) {
  void* pixels;
  int pitch;
  if (SDL_LockTexture(s_simulation_texture, nullptr, &pixels, &pitch) != 0) {
      return;
  }

  Uint32* dst = static_cast<Uint32*>(pixels);

  int end_x = s_position_x + s_width;

  int index = 0;
  for (int y = s_position_y; y < s_position_y + s_height; ++y) {
    for (int x = s_position_x; x < s_position_x + s_width; ++x) {
      SDL_Color color;
      if (matrix.isInBounds(x, y)) {
        color = matrix.getElement(x, y).color;
      }
      else {
        color = { 255, 255, 255, 255 };
      }  
      int dir = Matrix::getStep() ? 1 : -1;
      if (Matrix::getDebugIndex() + dir == x + y * Simulation::WIDTH) color = { 255, 0, 0, 255 };

      dst[index] = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;

      ++index;
    }
  }

  SDL_UnlockTexture(s_simulation_texture);
}

void Viewport::addToPosition(const int delta_x, const int delta_y) {
  s_position_x += delta_x;
  s_position_y += delta_y;
}

void Viewport::setZoomLevel(int zoom) {
  zoom = std::clamp(zoom, 1, 80);  
  if (zoom == s_zoom) return;
  int new_width = 16 * zoom;
  int new_height = 9 * zoom;

  int delta_x = (s_width - new_width) / 2;
  int delta_y = (s_height - new_height) / 2;

  s_position_x += delta_x;
  s_position_y += delta_y;

  if (s_simulation_texture) {
    SDL_DestroyTexture(s_simulation_texture);
  }

  // Recreate the texture with the current width and new height
    s_simulation_texture = SDL_CreateTexture(
      Renderer::getRenderer(), // You may need to provide access to your SDL_Renderer
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING,
      new_width,
      new_height
    );
    SDL_SetTextureBlendMode(s_simulation_texture, SDL_BLENDMODE_BLEND);
    
    s_width = new_width;
    s_height = new_height;
    s_zoom = zoom;
}

int Viewport::getWidth() {
  return s_width;
}

int Viewport::getHeight() {
  return s_height;
}

int Viewport::getPositionX() {
  return s_position_x;
}

int Viewport::getPositionY() {
  return s_position_y;
}

int Viewport::getZoom() {
  return s_zoom;
}