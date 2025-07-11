// src/core/Chunk.hpp
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>

class Chunk {
public:
	static const int CHUNK_SIZE = 16; // 16x16 chunks for 320x200 = 20x13 chunks
	
	Chunk(int chunkX, int chunkY);
	Chunk();
	
	// Activity management
	bool isActive() const { return active; }
	void activate();
	void deactivate();
	void setActiveNextFrame() { activeNextFrame = true; }
	
	// Update activity state for next frame
	void updateActivityState();
	
	// Check if any elements in this chunk moved this frame
	bool hasMovingElements() const { return hasMovement; }
	void setHasMovement(bool movement) { hasMovement = movement; }
	void resetMovementFlag() { hasMovement = false; }
	
	// Chunk coordinates
	int getChunkX() const { return chunkX; }
	int getChunkY() const { return chunkY; }
	
	// Get world coordinates of this chunk
	int getWorldX() const { return chunkX * CHUNK_SIZE; }
	int getWorldY() const { return chunkY * CHUNK_SIZE; }
	
private:
	int chunkX, chunkY;
	bool active;
	bool activeNextFrame;
	bool hasMovement;
};

#endif // CHUNK_HPP