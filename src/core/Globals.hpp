#ifndef GLOBALS_HPP
#define GLOBALS_HPP

namespace Window {
	const static int WIDTH = 512; //1152;
	const static int HEIGHT = 512; //648;
};

namespace Simulation {
	const static int WIDTH = 512; //384;
	const static int HEIGHT = 512; //216;
};

namespace Chunks {
	const static int CHUNK_SIZE = 32;
	const static int CHUNKS_X = (Simulation::WIDTH + CHUNK_SIZE - 1) / CHUNK_SIZE;
	const static int CHUNKS_Y = (Simulation::HEIGHT + CHUNK_SIZE - 1) / CHUNK_SIZE;
};

const static float g_PHYSICS_HZ = 60.0f;
const static float g_MS_PER_UPDATE = 1000.0f / g_PHYSICS_HZ;

#endif // GLOBALS_HPP