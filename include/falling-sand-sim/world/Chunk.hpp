// Chunk.hpp
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
// Declares the Chunk class for managing simulation chunk boundaries and
// update rectangles.

#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <SDL2/SDL.h>

class Chunk {
public:	
	Chunk(const int chunk_x, const int chunk_y);
	Chunk();
	~Chunk() = default;

	int getChunkX() const;
	int getChunkY() const;

	int getLeftX() const;
	int getRightX() const;
	
	int getTopY() const;
	int getBottomY() const;

	void addNewPosition(const int x, const int y);
	void updateWholeChunk();

	void commitUpdateRect();

	const SDL_Rect& getCurrentUpdateRect() const;
	const SDL_Rect& getLastUpdateRect() const;

private:
	int m_chunk_x, m_chunk_y;
	int m_left_x, m_right_x;
	int m_top_y, m_bottom_y;
	SDL_Rect m_current_update_rect;
	SDL_Rect m_last_update_rect;
	SDL_Rect m_next_update_rect;
	bool updated_rect;
};

#endif // CHUNK_HPP