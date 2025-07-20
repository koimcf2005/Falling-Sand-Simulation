// src/core/Globals.hpp
#ifndef GLOBALS_HPP
#define GLOBALS_HPP

class Renderer;  // Forward declaration

extern Renderer* g_Renderer;

namespace Window {
	const static int WIDTH = 1152;
	const static int HEIGHT = 648;
};

namespace Matrix {
	const static int WIDTH = 384;
	const static int HEIGHT = 216;
};

const static int g_CHUNK_SIZE = 8;

const static int g_CHUNKS_X = (Matrix::WIDTH + g_CHUNK_SIZE - 1) / g_CHUNK_SIZE;
const static int g_CHUNKS_Y = (Matrix::HEIGHT + g_CHUNK_SIZE - 1) / g_CHUNK_SIZE;

const static float g_PHYSICS_HZ = 5.0f;
const static float g_MS_PER_UPDATE = 1000.0f / g_PHYSICS_HZ;

#endif // GLOBALS_HPP