// src/core/CellularMatrix.cpp
#include "src/core/CellularMatrix.hpp"
#include "src/elements/types/Empty.hpp"
#include "src/core/Globals.hpp"
#include "src/core/Renderer.hpp"
#include <algorithm>
#include <random>
#include <utility>
#include <iostream>

//-------------------------------------------
// Construction/Destruction
//-------------------------------------------
CellularMatrix::CellularMatrix(int width, int height)
	: pixels(width * height)
{
	// Initialize chunks before matrix ---
	for (int chunkY = 0; chunkY < g_CHUNKS_Y; ++chunkY) {
		for (int chunkX = 0; chunkX < g_CHUNKS_X; ++chunkX) {
			chunks[chunkY][chunkX] = Chunk(chunkX, chunkY);
		}
	}

	// --- Use reserve for matrix rows to avoid reallocations ---
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			matrix[y][x] = new Empty(x, y); 
		}
	}
}

CellularMatrix::~CellularMatrix() {
	// Clean up SDL resources
	if (renderTexture) {
		SDL_DestroyTexture(renderTexture);
	}
	
	// Clean up all elements in the grid
	for (int y = 0; y < Matrix::HEIGHT; ++y) {
		for (int x = 0; x < Matrix::WIDTH; ++x) {
			if (matrix[y][x]) {
				delete matrix[y][x];
				matrix[y][x] = nullptr;
			}
		}
	}
}

//-------------------------------------------
// IMatrixAccess Implementation
//-------------------------------------------
bool CellularMatrix::isInBounds(int x, int y) const {
	return x >= 0 && x < Matrix::WIDTH && y >= 0 && y < Matrix::HEIGHT;
}

bool CellularMatrix::isEmpty(int x, int y) const {
	return matrix[y][x]->getType() == EMPTY;
}

Element*& CellularMatrix::getElement(int x, int y) {
	return matrix[y][x];
}

const Element* CellularMatrix::getElement(int x, int y) const {
	return matrix[y][x];
}

void CellularMatrix::destroyElement(int x, int y) {
	if (matrix[y][x]->getType() != EMPTY) {
		Element* emptyElement = ElementFactory::createElementFromType(EMPTY, x, y);
		Element* oldElement = matrix[y][x];
		matrix[y][x] = emptyElement;
		delete oldElement;
	}
}

//-------------------------------------------
// Rendering Setup
//-------------------------------------------
void CellularMatrix::initializeTexture(SDL_Renderer* renderer) {
	// Clean up existing texture if any
	if (renderTexture) {
		SDL_DestroyTexture(renderTexture);
	}

	// Create streaming texture for efficient updates
	renderTexture = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		Matrix::WIDTH,
		Matrix::HEIGHT
	);
	SDL_SetTextureBlendMode(renderTexture, SDL_BLENDMODE_BLEND);
}

//-------------------------------------------
// Element Management
//-------------------------------------------
void CellularMatrix::placeElement(int x, int y, ElementType type) {
	if (x >= 0 && x < Matrix::WIDTH && y >= 0 && y < Matrix::HEIGHT) {
		if (matrix[y][x] && matrix[y][x]->getType() == type) {
			return;
		}
		Element* newElement = ElementFactory::createElementFromType(type, x, y);
		delete matrix[y][x]; // delete old element
		matrix[y][x] = newElement;
		
		// Activate the chunk containing this element
		activateChunk(x, y);
	}
}

void CellularMatrix::placeElementsInArea(int centerX, int centerY, int radius, ElementType type) {
	int r2 = std::max(1, radius * radius - 1);
	if (r2 == 1) {
		placeElement(centerX, centerY, type);
		return;
	}
	for (int y = centerY - radius; y <= centerY + radius; ++y) {
		for (int x = centerX - radius; x <= centerX + radius; ++x) {
			int dx = x - centerX;
			int dy = y - centerY;
			if (dx * dx + dy * dy <= r2) {
				placeElement(x, y, type);
			}
		}
	}
}

// void CellularMatrix::heatElementsInArea(int centerX, int centerY, int radius) {
// 	int r2 = radius * radius;
// 	for (int y = centerY - radius; y <= centerY + radius; ++y) {
// 		for (int x = centerX - radius; x <= centerX + radius; ++x) {
// 			int dx = x - centerX;
// 			int dy = y - centerY;
// 			if (dx * dx + dy * dy <= r2) {
// 				if (isInBounds(x, y)) {
// 					matrix[y][x]->setTemperature(10000);
// 					activateChunk(x, y);
// 				}
// 			}
// 		}
// 	}
// }

void CellularMatrix::swapElements(int x1, int y1, int x2, int y2) {
	std::swap(matrix[y1][x1], matrix[y2][x2]);
	matrix[y1][x1]->setPosition(x1, y1);
	matrix[y2][x2]->setPosition(x2, y2);

	// Mark both as updated for this frame to prevent double-update
	matrix[y1][x1]->setAsUpdated();
	matrix[y2][x2]->setAsUpdated();

	int chunk1X = getChunkX(x1), chunk1Y = getChunkY(y1);
	int chunk2X = getChunkX(x2), chunk2Y = getChunkY(y2);
	
	if (isValidChunk(chunk1X, chunk1Y)) {
		chunks[chunk1Y][chunk1X].activate();
		activateNeighboringChunks(x1, y1);
	}
	if (chunk1X == chunk2X && chunk1Y == chunk2Y) return;
	if (isValidChunk(chunk2X, chunk2Y)) {
		chunks[chunk2Y][chunk2X].activate();
		activateNeighboringChunks(x2, y2);
	}
}

//-------------------------------------------
// Chunk Management
//-------------------------------------------
void CellularMatrix::activateChunk(int x, int y) {
	int chunkX = getChunkX(x);
	int chunkY = getChunkY(y);
	if (isValidChunk(chunkX, chunkY)) {
		chunks[chunkY][chunkX].activate();
	}
	activateNeighboringChunks(x, y);
}

void CellularMatrix::activateNeighboringChunks(int x, int y) {
	bool onLeftEdge   = (x % g_CHUNK_SIZE) == 0;
	bool onRightEdge  = (x % g_CHUNK_SIZE) == g_CHUNK_SIZE - 1;
	bool onTopEdge    = (y % g_CHUNK_SIZE) == 0;
	bool onBottomEdge = (y % g_CHUNK_SIZE) == g_CHUNK_SIZE - 1;

	int chunkX = getChunkX(x);
	int chunkY = getChunkY(y);

	if (onLeftEdge && isValidChunk(chunkX - 1, chunkY)) {
		chunks[chunkY][chunkX - 1].activate();
	}
	if (onRightEdge && isValidChunk(chunkX + 1, chunkY)) {
		chunks[chunkY][chunkX + 1].activate();
	}
	if (onTopEdge && isValidChunk(chunkX, chunkY - 1)) {
		chunks[chunkY - 1][chunkX].activate();
	}
	if (onBottomEdge && isValidChunk(chunkX, chunkY + 1)) {
		chunks[chunkY + 1][chunkX].activate();
	}
}

bool CellularMatrix::isValidChunk(int chunkX, int chunkY) const {
	return chunkX >= 0 && chunkX < g_CHUNKS_X && chunkY >= 0 && chunkY < g_CHUNKS_Y;
}

int CellularMatrix::getActiveChunkCount() const {
	int count = 0;
	for (int chunkY = 0; chunkY < g_CHUNKS_Y; ++chunkY) {
		for (int chunkX = 0; chunkX < g_CHUNKS_X; ++chunkX) {
			if (chunks[chunkY][chunkX].isActive()) {
				count++;
			}
		}
	}
	return count;
}

void CellularMatrix::switchDebugMode() {
	debugMode = !debugMode;
}

//-------------------------------------------
// Simulation Update
//-------------------------------------------
void CellularMatrix::update() {
	static int columnOrder[Matrix::WIDTH];
	for (int x = 0; x < Matrix::WIDTH; ++x) columnOrder[x] = x;

	for (int y = Matrix::HEIGHT - 1; y >= 0; --y) {
		std::shuffle(columnOrder, columnOrder + Matrix::WIDTH, rng);

		for (int x : columnOrder) {
			int chunkX = getChunkX(x);
			int chunkY = getChunkY(y);
			if (chunks[chunkY][chunkX].isActive()) {
				matrix[y][x]->update(*this);
			}
		}
	}
	for (auto& row : chunks) {
		for (auto& chunk : row) {
			chunk.updateActivityState();
		}
	}
	ParticleManager::updateParticles();
	Element::s_Step = !Element::s_Step;
}

void CellularMatrix::updateChunk(int chunkX, int chunkY) {
	int startX = chunkX * g_CHUNK_SIZE;
	int startY = chunkY * g_CHUNK_SIZE;
	int endX = std::min(startX + g_CHUNK_SIZE, Matrix::WIDTH);
	int endY = std::min(startY + g_CHUNK_SIZE, Matrix::HEIGHT);

	// Create column order for this chunk
	std::vector<int> columnOrder;
	for (int x = startX; x < endX; ++x) {
		columnOrder.push_back(x);
	}
	
	// Process from bottom to top
	for (int y = endY - 1; y >= startY; --y) {
		std::shuffle(columnOrder.begin(), columnOrder.end(), rng);
		
		for (int x : columnOrder) {
			matrix[y][x]->update(*this);
		}
	}
}

//-------------------------------------------
// Rendering
//-------------------------------------------
void CellularMatrix::updateTexture() {
	// Convert element colors to pixel format
	for (int y = Matrix::HEIGHT - 1; y >= 0; --y) {
		for (int x = 0; x < Matrix::WIDTH; ++x) {
			SDL_Color color = matrix[y][x]->getColor();

			pixels[y * Matrix::WIDTH + x] = (color.r << 24) | (color.g << 16) | 
								   (color.b << 8) | color.a;
		}
	}

	if (debugMode) {
		for (int chunkY = g_CHUNKS_Y - 1; chunkY >= 0; --chunkY) {
			for (int chunkX = 0; chunkX < g_CHUNKS_X; ++chunkX) {
				Chunk chunk = chunks[chunkY][chunkX];
				if (chunk.isActive()) {
					g_Renderer->drawScreenSpaceRect(chunk.getWorldX(), chunk.getWorldY(), g_CHUNK_SIZE, g_CHUNK_SIZE, 1);
				}
			}
		}
	}

	for (size_t i = 0; i < ParticleManager::size(); ++i) {
		Particle& p = *ParticleManager::getParticle(i);
		for (int dy = 0; dy < p.height; ++dy) {
			for (int dx = 0; dx < p.width; ++dx) {
				int px = p.x + dx;
				int py = p.y + dy;

				if (px >= 0 && px < Matrix::WIDTH && py >= 0 && py < Matrix::HEIGHT) {
					int index = py * Matrix::WIDTH + px;

					// Extract background color
					Uint32 bg = pixels[index];
					Uint8 bg_r = (bg >> 24) & 0xFF;
					Uint8 bg_g = (bg >> 16) & 0xFF;
					Uint8 bg_b = (bg >> 8) & 0xFF;
					Uint8 bg_a = bg & 0xFF;

					// Particle color and alpha
					Uint8 fg_r = p.color.r;
					Uint8 fg_g = p.color.g;
					Uint8 fg_b = p.color.b;
					Uint8 fg_a = p.color.a;

					// Alpha blending (premultiplied alpha, "over" operator)
					float alpha = fg_a / 255.0f;
					float inv_alpha = 1.0f - alpha;

					Uint8 out_r = static_cast<Uint8>(fg_r * alpha + bg_r * inv_alpha);
					Uint8 out_g = static_cast<Uint8>(fg_g * alpha + bg_g * inv_alpha);
					Uint8 out_b = static_cast<Uint8>(fg_b * alpha + bg_b * inv_alpha);
					Uint8 out_a = std::max(fg_a, bg_a); // Opaque output (or use max(fg_a, bg_a) if you want)

					pixels[index] = (out_r << 24) | (out_g << 16) | (out_b << 8) | out_a;
				}
			}
		}
	}

	// Update and render texture
	SDL_UpdateTexture(renderTexture, NULL, pixels.data(), Matrix::WIDTH * sizeof(Uint32));
}

SDL_Texture* CellularMatrix::getTexture() const {
	return renderTexture;
}