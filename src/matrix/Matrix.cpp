#include "src/matrix/Matrix.hpp"

#include "src/elements/ElementFactory.hpp"
#include "src/renderer/Renderer.hpp"
#include "src/core/Utilities.hpp"

#include <iostream>
#include <vector>
#include <set>

//-------------------------------------------
// Constructor and Destructor
//-------------------------------------------

Matrix::Matrix()
: m_matrix{},
	m_chunks{},
	m_texture_pixels{},
	m_simulation_texture{nullptr},
	m_debug_mode{false}
{
	for (int chunk_y = 0; chunk_y < Chunks::CHUNKS_Y; ++chunk_y) {
		for (int chunk_x = 0; chunk_x < Chunks::CHUNKS_X; ++chunk_x) {
			m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X] = Chunk(chunk_x, chunk_y);
		}
	}

	int element_count = 0;
	for (int y = 0; y < Simulation::HEIGHT; ++y) {
		for (int x = 0; x < Simulation::WIDTH; ++x) {
			m_matrix[x + y * Simulation::WIDTH] = ElementFactory::createElementByType(EMPTY, x, y);
			element_count++;
		}
	}
	std::cout << element_count << " Elements initialized." << std::endl;
}

Matrix::~Matrix() {
	if (m_simulation_texture) {
		SDL_DestroyTexture(m_simulation_texture);
	}
}

//-------------------------------------------
// Main update loop
//-------------------------------------------

void Matrix::update() {
	static int row_order[Simulation::WIDTH];
	for (int x = 0; x < Simulation::WIDTH; ++x) row_order[x] = x;

	for (int y = Simulation::HEIGHT - 1; y >= 0; --y) {
		std::shuffle(row_order, row_order + Simulation::WIDTH, RNG::s_rng);
		for (int x : row_order) {
			Chunk& chunk = getChunk(getChunkX(x), getChunkY(y));
			if (chunk.isActive()) {
				entt::entity entity = getEntity(x, y);
				Element& element = ComponentManager::getComponent<Element>(entity);
				if (element.step != s_matrix_step) {
					ElementFactory::updateElementByType(element.type, *this, entity);
					element.step = s_matrix_step;
				}
			}
		}
	}
	for (auto& chunk : m_chunks) chunk.updateActivityState();
	s_matrix_step = !s_matrix_step;
}

//-------------------------------------------
// Element Management 
//-------------------------------------------

bool Matrix::isInBounds(const int x, const int y) const {
	return x >= 0 && x < Simulation::WIDTH && y >= 0 && y < Simulation::HEIGHT;
}

bool Matrix::isEmpty(const int x, const int y) const {
	return getElement(x, y).type == EMPTY;
}

void Matrix::placeElement(const ElementType type, const int x, const int y) {
	if (!isInBounds(x, y)) return;
	entt::entity entity = getEntity(x, y);
	Element& element = ComponentManager::getComponent<Element>(entity);
	if (element.type == type) return;

	ComponentManager::destroyEntity(entity);
	m_matrix[x + y * Simulation::WIDTH] = ElementFactory::createElementByType(type, x, y);
	activateChunk(x, y);
}

void Matrix::placeElementsInArea(const ElementType type, const int x, const int y, const int radius) {
	int r2 = std::max(1, radius * radius - 1);
	if (r2 == 1) {
		placeElement(type, x, y);
		return;
	}
	for (int ty = y - radius; ty <= y + radius; ++ty) {
		for (int tx = x - radius; tx <= x + radius; ++tx) {
			int dx = tx - x;
			int dy = ty - y;
			if (dx * dx + dy * dy <= r2) {
				placeElement(type, tx, ty);
			}
		}
	}
}

void Matrix::swapEntities(const int x1, const int y1, const int x2, const int y2) {
	if (!isInBounds(x1, y1) || !isInBounds(x2, y2)) {
		return;
	}
	entt::entity element1 = m_matrix[x1 + y1 * Simulation::WIDTH];
	m_matrix[x1 + y1 * Simulation::WIDTH] = m_matrix[x2 + y2 * Simulation::WIDTH];
	m_matrix[x2 + y2 * Simulation::WIDTH] = element1;

	ComponentManager::getComponent<Position>(m_matrix[x1 + y1 * Simulation::WIDTH]) = {x1, y1};
	ComponentManager::getComponent<Position>(m_matrix[x2 + y2 * Simulation::WIDTH]) = {x2, y2};

	activateChunk(x1, y1);
	activateChunk(x2, y2);
}

void Matrix::activateChunk(const int x, const int y) {
	int chunk_x = getChunkX(x);
	int chunk_y = getChunkY(y);
	if (isValidChunk(chunk_x, chunk_y)) {
		Chunk& chunk = getChunk(chunk_x, chunk_y);
		chunk.activate();
		chunk.activateNextFrame();
	}
	activateNeighboringChunks(x, y);
}

//-------------------------------------------
// Rendering and Debug 
//-------------------------------------------

void Matrix::toggleDebugMode() { m_debug_mode = !m_debug_mode; }
SDL_Texture* Matrix::getTexture() const { return m_simulation_texture; }

void Matrix::initializeTexture(SDL_Renderer* renderer) {
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

void Matrix::updateTexture() {
	void* pixels;
	int pitch;
	if (SDL_LockTexture(m_simulation_texture, nullptr, &pixels, &pitch) != 0) {
			return;
	}

	Uint32* dst = static_cast<Uint32*>(pixels);

	for (auto& chunk : m_chunks) {
		if (!chunk.wasActive()) continue;
		for (int y = chunk.getTopY(); y <= chunk.getBottomY(); ++y) {
			for (int x = chunk.getLeftX(); x <= chunk.getRightX(); ++x) {
				int index = x + y * Simulation::WIDTH;
				SDL_Color color = getElement(x, y).color;
				dst[index] = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
			}
		}
	}

	SDL_UnlockTexture(m_simulation_texture);

	if (m_debug_mode) {
		for (auto& chunk : m_chunks) {
			if (!chunk.isActive()) continue;
			Renderer::drawScreenSpaceRect(
				chunk.getLeftX(),
				chunk.getTopY(),
				Chunks::CHUNK_SIZE,
				Chunks::CHUNK_SIZE,
				1
			);
		}
	}
}

//-------------------------------------------
// Global Static Step
//-------------------------------------------

bool Matrix::s_matrix_step = false;

bool Matrix::getStep() {
	return s_matrix_step;
}

//-------------------------------------------
// Chunk Getters
//-------------------------------------------

int Matrix::getChunkX(const int x) const { return x / Chunks::CHUNK_SIZE; }
int Matrix::getChunkY(const int y) const { return y / Chunks::CHUNK_SIZE; }

Chunk& Matrix::getChunk(const int chunk_x, const int chunk_y) { return m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X]; }
const Chunk& Matrix::getChunk(const int chunk_x, const int chunk_y) const { return m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X]; }

int Matrix::getActiveChunkCount() const {
	int count = 0;
	for (auto& chunk : m_chunks) {
		if (chunk.isActive()) ++count;
	}
	return count;
}

//-------------------------------------------
// Element Getters 
//-------------------------------------------

Element& Matrix::getElement(const int x, const int y) {
	if (!isInBounds(x, y)) {
		throw std::out_of_range("Matrix::getElement: coordinates out of bounds");
	}
	return ComponentManager::getComponent<Element>(m_matrix[x + y * Simulation::WIDTH]);
}


Element& Matrix::getElement(const int x, const int y) const {
	if (!isInBounds(x, y)) {
		throw std::out_of_range("Matrix::getElement: coordinates out of bounds");
	}
	return ComponentManager::getComponent<Element>(m_matrix[x + y * Simulation::WIDTH]);
}

//-------------------------------------------
// Entity Getters 
//-------------------------------------------

entt::entity Matrix::getEntity(const int x, const int y) {
	if (!isInBounds(x, y)) {
		return entt::null;
	}
	return m_matrix[x + y * Simulation::WIDTH];
}

entt::entity Matrix::getEntity(const int x, const int y) const {
	if (!isInBounds(x, y)) {
		return entt::null;
	}
	return m_matrix[x + y * Simulation::WIDTH];
}

//-------------------------------------------
// Chunk helper functions
//-------------------------------------------

void Matrix::activateNeighboringChunks(const int x, const int y) {
	bool onLeftEdge   = (x % Chunks::CHUNK_SIZE) == 0;
	bool onRightEdge  = (x % Chunks::CHUNK_SIZE) == Chunks::CHUNK_SIZE - 1;
	bool onTopEdge    = (y % Chunks::CHUNK_SIZE) == 0;
	bool onBottomEdge = (y % Chunks::CHUNK_SIZE) == Chunks::CHUNK_SIZE - 1;

	int chunk_x = getChunkX(x);
	int chunk_y = getChunkY(y);

	if (onLeftEdge && isValidChunk(chunk_x - 1, chunk_y)) {
		getChunk(chunk_x - 1, chunk_y).activate();
	}
	if (onRightEdge && isValidChunk(chunk_x + 1, chunk_y)) {
		getChunk(chunk_x + 1, chunk_y).activate();
	}
	if (onTopEdge && isValidChunk(chunk_x, chunk_y - 1)) {
		getChunk(chunk_x, chunk_y - 1).activate();
	}
	if (onBottomEdge && isValidChunk(chunk_x, chunk_y + 1)) {
		getChunk(chunk_x, chunk_y + 1).activate();
	}
}

bool Matrix::isValidChunk(const int chunk_x, const int chunk_y) const {
	return chunk_x >= 0 && chunk_x < Chunks::CHUNKS_X && chunk_y >= 0 && chunk_y < Chunks::CHUNKS_Y;
}