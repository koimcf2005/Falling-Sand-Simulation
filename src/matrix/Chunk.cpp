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
	m_dirty_rect{0, 0, 0, 0},
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
	m_dirty_rect{0, 0, 0, 0},
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
	if (x > m_dirty_rect.x) m_dirty_rect.x = x;
}

bool Chunk::updateActivityState() {
	m_dirty_rect = {0, 0, 0, 0};
	m_was_active = m_active;
	m_active = m_active_next_frame;
	m_active_next_frame = false;
	return m_active;
}