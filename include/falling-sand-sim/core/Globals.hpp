// Globals.hpp
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
// Defines global constants for window, simulation, and chunk dimensions and
// timing. Used throughout the simulation for consistent configuration.

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

namespace Window {
	const static int WIDTH = 512; 
	const static int HEIGHT = 512;
};

namespace Simulation {
	const static int WIDTH = 512/4;
	const static int HEIGHT = 512/4;
  
  const static float PHYSICS_HZ = 60.0f;
  const static float MS_PER_UPDATE = 1000.0f / Simulation::PHYSICS_HZ;
};

namespace Chunks {
	const static int CHUNK_SIZE = 64;
	const static int CHUNKS_X = (Simulation::WIDTH + CHUNK_SIZE - 1) / CHUNK_SIZE;
	const static int CHUNKS_Y = (Simulation::HEIGHT + CHUNK_SIZE - 1) / CHUNK_SIZE;
};

#endif // GLOBALS_HPP