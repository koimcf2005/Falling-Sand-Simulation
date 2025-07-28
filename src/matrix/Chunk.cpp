#include "src/matrix/Chunk.hpp"

#include "src/core/Globals.hpp"

#include <algorithm> 

Chunk::Chunk(const int chunk_x, const int chunk_y) 
: m_chunk_x{chunk_x},
	m_chunk_y{chunk_y},
	m_left_x{m_chunk_x * Chunks::CHUNK_SIZE},
	m_right_x{std::min(m_chunk_x * Chunks::CHUNK_SIZE + Chunks::CHUNK_SIZE - 1, Simulation::WIDTH - 1)},
	m_top_y{m_chunk_y * Chunks::CHUNK_SIZE},
	m_bottom_y{std::min(m_chunk_y * Chunks::CHUNK_SIZE + Chunks::CHUNK_SIZE - 1, Simulation::HEIGHT - 1)},
	m_dirty_rect{-1, -1, -1, -1},
	m_old_dirty_rect{-1, -1, -1, -1},
	updated_rect{false},
	m_active{true},
	m_was_active{false},
	m_active_next_frame{false}
{}

Chunk::Chunk() 
: m_chunk_x{0},
	m_chunk_y{0},
	m_left_x{0},
	m_right_x{Chunks::CHUNK_SIZE - 1},
	m_top_y{0},
	m_bottom_y{Chunks::CHUNK_SIZE - 1},
	m_dirty_rect{-1, -1, -1, -1},
	m_old_dirty_rect{-1, -1, -1, -1},
	updated_rect{false},
	m_active{true},
	m_was_active{false},
	m_active_next_frame{false}
{}

bool Chunk::wasActive() const { return m_was_active; }
bool Chunk::isActive() const { return m_active; }
bool Chunk::isActiveNextFrame() const { return m_active_next_frame; }

void Chunk::activate() { m_active = true; }
void Chunk::deactivate() { m_active = false; }

void Chunk::activateNextFrame() { m_active_next_frame = true; }
void Chunk::deactivateNextFrame() { m_active_next_frame = false; }

int Chunk::getChunkX() const { return m_chunk_x; }
int Chunk::getChunkY() const { return m_chunk_y; }

int Chunk::getLeftX() const { return m_left_x; }
int Chunk::getRightX() const { return m_right_x; }

int Chunk::getTopY() const { return m_top_y; }
int Chunk::getBottomY() const { return m_bottom_y; }

void Chunk::updateRect(const int x, const int y) {
	// If the dirty rect is still the default (full chunk), reset it to a 1x1 rect at (x, y)
	if (!updated_rect) {
			m_dirty_rect = {x, y, 1, 1};
			updated_rect = true;
			return;
	}

	// Expand the dirty rect to include the new point (x, y)
	const int right = m_dirty_rect.x + m_dirty_rect.w - 1;
	const int bottom = m_dirty_rect.y + m_dirty_rect.h - 1;

	if (x < m_dirty_rect.x) {
			m_dirty_rect.w += (m_dirty_rect.x - x);
			m_dirty_rect.x = x;
	} else if (x > right) {
			m_dirty_rect.w = (x - m_dirty_rect.x + 1);
	}

	if (y < m_dirty_rect.y) {
			m_dirty_rect.h += (m_dirty_rect.y - y);
			m_dirty_rect.y = y;
	} else if (y > bottom) {
			m_dirty_rect.h = (y - m_dirty_rect.y + 1);
	}
}

void Chunk::resetDirtyRect() {
	m_old_dirty_rect = m_dirty_rect;
	m_dirty_rect = {-1, -1, -1, -1};
	updated_rect = false;
}

bool Chunk::updateActivityState() {
	resetDirtyRect();
	m_was_active = m_active;
	m_active = m_active_next_frame;
	m_active_next_frame = false;
	return m_active;
}

const SDL_Rect& Chunk::getDirtyRect() const {
	return m_dirty_rect;
}

const SDL_Rect& Chunk::getOldDirtyRect() const {
	return m_old_dirty_rect;
}