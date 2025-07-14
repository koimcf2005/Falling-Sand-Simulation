// src/core/CellularMatrix.hpp
#ifndef CELLULARMATRIX_HPP
#define CELLULARMATRIX_HPP

#include "src/core/IMatrix.hpp"
#include "src/core/Chunk.hpp"
#include "src/elements/Element.hpp"
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
	void initializeRenderer(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer);

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
	std::vector<std::vector<Element*>> matrix;
	const int WIDTH;
	const int HEIGHT;

	// Chunk system
	const int CHUNKS_X;
	const int CHUNKS_Y;
	std::vector<std::vector<Chunk>> chunks;
	
	// Rendering
	SDL_Texture* renderTexture = nullptr;
	std::vector<Uint32> pixels;
	
	// Random number generation
	std::mt19937 rng{std::random_device{}()};
	
	// Debug
	bool debugMode = false;

	// Helper methods
	int getChunkX(int worldX) const { return worldX / Chunk::CHUNK_SIZE; }
	int getChunkY(int worldY) const { return worldY / Chunk::CHUNK_SIZE; }
	bool isValidChunk(int chunkX, int chunkY) const;
	void updateChunk(int chunkX, int chunkY);
};

#endif // CELLULARMATRIX_HPP