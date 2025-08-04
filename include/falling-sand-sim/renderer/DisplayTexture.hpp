// DisplayTexture.hpp
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
// Declares the SimulationTexture class for managing the simulation's display
// texture and chunk overlays.

#ifndef DISPLAY_TEXTURE_HPP
#define DISPLAY_TEXTURE_HPP

#include "falling-sand-sim/world/Matrix.hpp"

#include <SDL2/SDL.h>

class SimulationTexture {
public:
  SDL_Texture* getTexture() const;
  void initializeTexture(SDL_Renderer* renderer);
  void updateTexture(const Matrix& matrix);

  void toggleShowChunks();

private:
  SDL_Texture* m_simulation_texture;
    
  bool m_show_chunks;

  bool m_debug_mode;

};

#endif // DISPLAY_TEXTURE_HPP