// Chunk.cpp
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
// Implements the Chunk class for managing simulation chunk boundaries and
// update rectangles.

#include "falling-sand-sim/world/Chunk.hpp"

#include "falling-sand-sim/core/Globals.hpp"

#include <algorithm> 

Chunk::Chunk(const int chunk_x, const int chunk_y) 
: m_chunk_x{chunk_x},
	m_chunk_y{chunk_y},
	m_left_x{m_chunk_x * Chunks::CHUNK_SIZE},
	m_right_x{std::min(m_chunk_x * Chunks::CHUNK_SIZE + Chunks::CHUNK_SIZE - 1, Simulation::WIDTH - 1)},
	m_top_y{m_chunk_y * Chunks::CHUNK_SIZE},
	m_bottom_y{std::min(m_chunk_y * Chunks::CHUNK_SIZE + Chunks::CHUNK_SIZE - 1, Simulation::HEIGHT - 1)},
	m_current_update_rect{0, 0, 0, 0},
	m_last_update_rect{0, 0, 0, 0},
	m_next_update_rect{0, 0, 0, 0},
	updated_rect{false}
{}

Chunk::Chunk() 
: m_chunk_x{0},
	m_chunk_y{0},
	m_left_x{0},
	m_right_x{Chunks::CHUNK_SIZE - 1},
	m_top_y{0},
	m_bottom_y{Chunks::CHUNK_SIZE - 1},
	m_current_update_rect{0, 0, 0, 0},
	m_last_update_rect{0, 0, 0, 0},
	m_next_update_rect{0, 0, 0, 0},
	updated_rect{false}
{}

int Chunk::getChunkX() const { return m_chunk_x; }
int Chunk::getChunkY() const { return m_chunk_y; }

int Chunk::getLeftX() const { return m_left_x; }
int Chunk::getRightX() const { return m_right_x; }

int Chunk::getTopY() const { return m_top_y; }
int Chunk::getBottomY() const { return m_bottom_y; }

void Chunk::addNewPosition(const int x, const int y) {
	if (!updated_rect) {
		m_next_update_rect = {x, y, 1, 1};
		updated_rect = true;
		return;
	}

	int left   = std::min(m_next_update_rect.x, x);
	int right  = std::max(m_next_update_rect.x + m_next_update_rect.w - 1, x);
	int top    = std::min(m_next_update_rect.y, y);
	int bottom = std::max(m_next_update_rect.y + m_next_update_rect.h - 1, y);

	m_next_update_rect.x = left;
	m_next_update_rect.y = top;
	m_next_update_rect.w = right - left + 1;
	m_next_update_rect.h = bottom - top + 1;
}

void Chunk::updateWholeChunk() {
	m_next_update_rect.x = getLeftX();
	m_next_update_rect.w = Chunks::CHUNK_SIZE;
	m_next_update_rect.y = getTopY();
	m_next_update_rect.h = Chunks::CHUNK_SIZE;
}

void Chunk::commitUpdateRect() {
	m_last_update_rect = m_current_update_rect;
	m_current_update_rect = m_next_update_rect;
	m_next_update_rect = {0, 0, 0, 0};
	updated_rect = false;
}

const SDL_Rect& Chunk::getCurrentUpdateRect() const {
	return m_current_update_rect;
}

const SDL_Rect& Chunk::getLastUpdateRect() const {
	return m_last_update_rect;
}