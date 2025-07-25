// src/core/Chunk.cpp
#include "src/core/Chunk.hpp"
#include "src/core/Globals.hpp"
#include "src/core/Renderer.hpp"

Chunk::Chunk(int chunkX, int chunkY) 
	: chunkX(chunkX), chunkY(chunkY), active(true), activeNextFrame(false) {
}

Chunk::Chunk() 
	: chunkX(0), chunkY(0), active(true), activeNextFrame(false) {
}

bool Chunk::isActive() const {
	return active;
}

void Chunk::activate() {
	active = true;
	activeNextFrame = true;
}

void Chunk::deactivate() {
	active = false;
	activeNextFrame = false;
}

void Chunk::updateActivityState() {
	if (activeNextFrame) {
		active = true;
		activeNextFrame = false;
		countdown = 10;
	} else if (active) {
		if (--countdown <= 0) {
			active = false;
		}
	}
}

// Chunk coordinates
int Chunk::getChunkX() const {
	return chunkX;
}

int Chunk::getChunkY() const {
	return chunkY;
}

int Chunk::getWorldX() const {
	return chunkX * g_CHUNK_SIZE;
}

int Chunk::getWorldY() const {
	return chunkY * g_CHUNK_SIZE;
}