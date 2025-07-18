// src/core/CellularMatrix.cpp
#include "src/core/CellularMatrix.hpp"
#include "src/elements/EmptyElement.hpp"
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
	: matrix(height),
	WIDTH{width},
	HEIGHT{height},
	CHUNKS_X((width + Chunk::CHUNK_SIZE - 1) / Chunk::CHUNK_SIZE),
	CHUNKS_Y((height + Chunk::CHUNK_SIZE - 1) / Chunk::CHUNK_SIZE),
	chunks(CHUNKS_Y),
	pixels(width * height)
{
	// Initialize the grid with empty elements
	for (int y = 0; y < height; ++y) {
		matrix[y].resize(width);
		for (int x = 0; x < width; ++x) {
			matrix[y][x] = new Empty(x, y); 
		}
	}
	
	// Initialize chunks
	for (int chunkY = 0; chunkY < CHUNKS_Y; ++chunkY) {
		chunks[chunkY].resize(CHUNKS_X);
		for (int chunkX = 0; chunkX < CHUNKS_X; ++chunkX) {
			chunks[chunkY][chunkX] = Chunk( chunkX, chunkY);
		}
	}
}

CellularMatrix::~CellularMatrix() {
	// Clean up SDL resources
	if (renderTexture) {
		SDL_DestroyTexture(renderTexture);
	}
	
	// Clean up all elements in the grid
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			delete matrix[y][x];
		}
	}
}

//-------------------------------------------
// IMatrixAccess Implementation
//-------------------------------------------
bool CellularMatrix::isInBounds(int x, int y) const {
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
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
void CellularMatrix::initializeRenderer(SDL_Renderer* renderer) {
	// Clean up existing texture if any
	if (renderTexture) {
		SDL_DestroyTexture(renderTexture);
	}

	// Create streaming texture for efficient updates
	renderTexture = SDL_CreateTexture(renderer, 
									SDL_PIXELFORMAT_RGBA8888,
									SDL_TEXTUREACCESS_STREAMING,
									WIDTH, HEIGHT);
}

//-------------------------------------------
// Element Management
//-------------------------------------------
void CellularMatrix::placeElement(int x, int y, ElementType type) {
	if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
		if (matrix[y][x] && matrix[y][x]->getType() == type) {
			return;
		}
		
		Element* newElement = ElementFactory::createElementFromType(type, x, y);
		delete matrix[y][x];
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

void CellularMatrix::heatElementsInArea(int centerX, int centerY, int radius) {
	int r2 = radius * radius;
	for (int y = centerY - radius; y <= centerY + radius; ++y) {
		for (int x = centerX - radius; x <= centerX + radius; ++x) {
			int dx = x - centerX;
			int dy = y - centerY;
			if (dx * dx + dy * dy <= r2) {
				if (isInBounds(x, y)) {
					matrix[y][x]->setTemperature(10000);
					activateChunk(x, y);
				}
			}
		}
	}
}

void CellularMatrix::swapElements(int x1, int y1, int x2, int y2) {
	std::swap(matrix[y1][x1], matrix[y2][x2]);
	
	// Mark chunks as having movement
	int chunk1X = getChunkX(x1), chunk1Y = getChunkY(y1);
	int chunk2X = getChunkX(x2), chunk2Y = getChunkY(y2);
	
	if (isValidChunk(chunk1X, chunk1Y)) {
		chunks[chunk1Y][chunk1X].activate();
		activateNeighboringChunks(x1, y1);
	}
	if ((chunk1X != chunk2X || chunk1Y != chunk2Y) && isValidChunk(chunk2X, chunk2Y)) {
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
	bool onLeftEdge   = (x % Chunk::CHUNK_SIZE) == 0;
	bool onRightEdge  = (x % Chunk::CHUNK_SIZE) == Chunk::CHUNK_SIZE - 1;
	bool onTopEdge    = (y % Chunk::CHUNK_SIZE) == 0;
	bool onBottomEdge = (y % Chunk::CHUNK_SIZE) == Chunk::CHUNK_SIZE - 1;

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
	return chunkX >= 0 && chunkX < CHUNKS_X && chunkY >= 0 && chunkY < CHUNKS_Y;
}

int CellularMatrix::getActiveChunkCount() const {
	int count = 0;
	for (int chunkY = 0; chunkY < CHUNKS_Y; ++chunkY) {
		for (int chunkX = 0; chunkX < CHUNKS_X; ++chunkX) {
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
	// Update only active chunks
	for (int chunkY = CHUNKS_Y - 1; chunkY >= 0; --chunkY) {
		for (int chunkX = 0; chunkX < CHUNKS_X; ++chunkX) {
			if (chunks[chunkY][chunkX].isActive()) {
				updateChunk(chunkX, chunkY);
			}
			chunks[chunkY][chunkX].updateActivityState();
		}
	}
	
	Element::globalStep = !Element::globalStep;
}

void CellularMatrix::updateChunk(int chunkX, int chunkY) {
	int startX = chunkX * Chunk::CHUNK_SIZE;
	int startY = chunkY * Chunk::CHUNK_SIZE;
	int endX = std::min(startX + Chunk::CHUNK_SIZE, WIDTH);
	int endY = std::min(startY + Chunk::CHUNK_SIZE, HEIGHT);

	// Create column order for this chunk
	std::vector<int> columnOrder;
	for (int x = startX; x < endX; ++x) {
		columnOrder.push_back(x);
	}
	
	// Process from bottom to top
	for (int y = endY - 1; y >= startY; --y) {
		std::shuffle(columnOrder.begin(), columnOrder.end(), rng);
		
		for (int x : columnOrder) {
			matrix[y][x]->update(*this, x, y);
		}
	}
}

//-------------------------------------------
// Rendering
//-------------------------------------------
void CellularMatrix::updateTexture() {
	// Convert element colors to pixel format
	for (int y = HEIGHT - 1; y >= 0; --y) {
		for (int x = 0; x < WIDTH; ++x) {
			SDL_Color color = matrix[y][x]->getColor();

			pixels[y * WIDTH + x] = (color.r << 24) | (color.g << 16) | 
								   (color.b << 8) | color.a;
		}
	}

	if (debugMode) {
		for (int chunkY = CHUNKS_Y - 1; chunkY >= 0; --chunkY) {
			for (int chunkX = 0; chunkX < CHUNKS_X; ++chunkX) {
				Chunk chunk = chunks[chunkY][chunkX];
				if (chunk.isActive()) {
					gRenderer->drawScreenSpaceRect(chunk.getWorldX(), chunk.getWorldY(), Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE, 1);
				}
			}
		}
	}

	// Update and render texture
	SDL_UpdateTexture(renderTexture, NULL, pixels.data(), WIDTH * sizeof(Uint32));
}

SDL_Texture* CellularMatrix::getTexture() const {
	return renderTexture;
}