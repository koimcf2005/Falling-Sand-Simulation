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
	bool isActive() const;
	void activate();
	void deactivate();
	
	// Update activity state for next frame
	void updateActivityState();
	
	// Chunk coordinates
	int getChunkX() const;
	int getChunkY() const;
	
private:
	int chunkX, chunkY;
	bool active;
	bool activeNextFrame;
	int countdown = 10;
};

#endif // CHUNK_HPP