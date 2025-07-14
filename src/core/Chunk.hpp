// src/core/Chunk.hpp
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>

class Chunk {
public:
	static const int CHUNK_SIZE = 8;
	
	Chunk(int chunkX, int chunkY);
	Chunk();
	
	// Activity management
	bool isActive() const;
	void activate();
	void deactivate();
	
	// Update activity state for next frame
	void updateActivityState();
	
	// Chunk coordinates
	int getChunkX() const;
	int getChunkY() const;

	// Get world coordinates of this chunk
	int getWorldX() const;
	int getWorldY() const;
	
private:
	int chunkX, chunkY;
	bool active;
	bool activeNextFrame;
	int countdown = 10;
};

#endif // CHUNK_HPP