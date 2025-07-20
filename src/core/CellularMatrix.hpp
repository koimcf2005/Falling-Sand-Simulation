// src/core/CellularMatrix.hpp
#ifndef CELLULARMATRIX_HPP
#define CELLULARMATRIX_HPP

#include "src/core/IMatrix.hpp"
#include "src/core/Chunk.hpp"
#include "src/core/Globals.hpp"
#include "src/elements/Element.hpp"
#include "src/particles/ParticleManager.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <random>

class CellularMatrix : public IMatrix {
public:
	CellularMatrix(int width, int height);
	~CellularMatrix();

	// IMatrix interface implementation
	bool isInBounds(int x, int y) const override;
	bool isEmpty(int x, int y) const override;
	Element*& getElement(int x, int y) override;
	const Element* getElement(int x, int y) const override;
	void destroyElement(int x, int y) override;
	void swapElements(int x1, int y1, int x2, int y2) override;

	// Rendering
	void initializeTexture(SDL_Renderer* renderer);
	void updateTexture();
	SDL_Texture* getTexture() const; // returns renderTexture

	// Element placement
	void placeElement(int x, int y, ElementType type) override;
	void placeElementsInArea(int centerX, int centerY, int radius, ElementType type);
	void heatElementsInArea(int centerX, int centerY, int radius);

	// Main update loop
	void update();

	// Chunk management
	void activateChunk(int x, int y) override;
	void activateNeighboringChunks(int chunkX, int chunkY);
	
	// Debug info
	void switchDebugMode();
	bool getDebugMode() const { return debugMode; }
	int getActiveChunkCount() const;

private:
	// Grid data
	Element* matrix[Matrix::HEIGHT][Matrix::WIDTH];

	// Chunk system
	Chunk chunks[g_CHUNKS_Y][g_CHUNKS_X];
	
	// Rendering
	SDL_Texture* renderTexture = nullptr;
	std::vector<Uint32> pixels;
	
	// Random number generation
	std::mt19937 rng{std::random_device{}()};
	
	// Debug
	bool debugMode = false;

	static bool globalStep;

	// Helper methods
	int getChunkX(int worldX) const { return worldX / g_CHUNK_SIZE; }
	int getChunkY(int worldY) const { return worldY / g_CHUNK_SIZE; }
	bool isValidChunk(int chunkX, int chunkY) const;
	void updateChunk(int chunkX, int chunkY);
	void updateChunkRow(int chunkX, int chunkY, int row);
};

#endif // CELLULARMATRIX_HPP