// Viewport.hpp
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
// Declares the Viewport class for managing the simulation's display
// texture and chunk overlays.

#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "falling-sand-sim/world/Matrix.hpp"

#include <SDL2/SDL.h>

class Viewport {
public:
  static SDL_Texture* getTexture();
  static void initialize(SDL_Renderer* renderer);
  static void updateTexture(const Matrix& matrix);

  static void addToPosition(const int delta_x, const int delta_y);
  static void setZoomLevel(int zoom);
  
  static int getWidth();
  static int getHeight();

  static int getPositionX();
  static int getPositionY();

  static int getZoom();

private:
  static SDL_Texture* s_simulation_texture;

  static int s_zoom;

  static int s_width;
  static int s_height;

  static int s_position_x;
  static int s_position_y;

};

#endif // VIEWPORT_HPP