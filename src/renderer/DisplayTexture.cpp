// DisplayTexture.cpp
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
// Implements the SimulationTexture class for managing the simulation's display
// texture and chunk overlays.

#include "falling-sand-sim/renderer/DisplayTexture.hpp"

#include "falling-sand-sim/core/Globals.hpp"
#include "falling-sand-sim/renderer/Renderer.hpp"

SDL_Texture* SimulationTexture::getTexture() const { return m_simulation_texture; }

/**
 * @brief Initializes the simulation texture for rendering.
 * @param renderer Pointer to the SDL_Renderer.
 */
void SimulationTexture::initializeTexture(SDL_Renderer* renderer) {
  // Clean up existing texture if any
  if (m_simulation_texture) {
    SDL_DestroyTexture(m_simulation_texture);
  }

  // Create streaming texture for efficient updates
  m_simulation_texture = SDL_CreateTexture(
    renderer, 
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING,
    Simulation::WIDTH,
    Simulation::HEIGHT
  );
  SDL_SetTextureBlendMode(m_simulation_texture, SDL_BLENDMODE_BLEND);
}

/**
 * @brief Updates the simulation texture with the current element states.
 */
void SimulationTexture::updateTexture(const Matrix& matrix) {
  void* pixels;
  int pitch;
  if (SDL_LockTexture(m_simulation_texture, nullptr, &pixels, &pitch) != 0) {
      return;
  }

  Uint32* dst = static_cast<Uint32*>(pixels);

  // for (const auto& chunk : m_chunks) {
  //     const SDL_Rect& rect = chunk.getCurrentUpdateRect();
  //     if (rect.w <= 0 || rect.h <= 0) continue; // skip empty rects
  //     int start_y = std::max(rect.y, 0);
  //     int end_y   = std::min(rect.y + rect.h, Simulation::HEIGHT);
  //     int start_x = std::max(rect.x, 0);
  //     int end_x   = std::min(rect.x + rect.w, Simulation::WIDTH);

  //     for (int y = start_y; y < end_y; ++y) {
  //         for (int x = start_x; x < end_x; ++x) {
  //             int index = x + y * Simulation::WIDTH;
  //             SDL_Color color = getElement(x, y).color;
  //             dst[index] = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
  //         }
  //     }
  // }

  for (int y = 0; y < Simulation::HEIGHT; ++y) {
    for (int x = 0; x < Simulation::WIDTH; ++x) {
      int index = x + y * Simulation::WIDTH;
      SDL_Color color = matrix.getElement(x, y).color;
      if (index == matrix.getDebugIndex()) color = { 255, 0, 0, 255 };
      dst[index] = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
    }
  }

  SDL_UnlockTexture(m_simulation_texture);

  if (m_show_chunks) {
    for (const auto& chunk : matrix.getChunks()) {
      Renderer::drawScreenSpaceRect(
        chunk.getLeftX(),
        chunk.getTopY(),
        Chunks::CHUNK_SIZE,
        Chunks::CHUNK_SIZE,
        1,
        {0, 0, 255, 255}
      );
      const SDL_Rect& rect = chunk.getCurrentUpdateRect();
      Renderer::drawScreenSpaceRect(
        rect.x,
        rect.y,
        rect.w,
        rect.h,
        1,
        {255, 0, 0, 255}
      );
    }
  }
}

void SimulationTexture::toggleShowChunks() {
  m_show_chunks = !m_show_chunks;
}