// src/core/Chunk.cpp
#include "src/core/Chunk.hpp"

Chunk::Chunk(int chunkX, int chunkY) 
    : chunkX(chunkX), chunkY(chunkY), active(true), activeNextFrame(false), hasMovement(false) {
}

Chunk::Chunk() 
    : chunkX(0), chunkY(0), active(true), activeNextFrame(false), hasMovement(false) {
}

void Chunk::activate() {
    active = true;
    activeNextFrame = false;
}

void Chunk::deactivate() {
    active = false;
    activeNextFrame = false;
}

void Chunk::updateActivityState() {
    if (activeNextFrame || hasMovement) {
        active = true;
        activeNextFrame = false;
    } else {
        active = false;
    }
    hasMovement = false;
}