#include "CellularMatrix.hpp"
#include "elements/Empty.hpp"
#include <algorithm>
#include <random>
#include <utility>

//-------------------------------------------
// Construction/Destruction
//-------------------------------------------
CellularMatrix::CellularMatrix(int width, int height, int chunkSize)
	: WIDTH{width},
	HEIGHT{height},
	CHUNK_SIZE(chunkSize),
	matrix(height),
	pixels(width * height)  // Pre-allocate pixel buffer for rendering
	// chunks(generateChunks())
{
	// Initialize the grid with empty elements
	for (int y = 0; y < height; ++y) {
		matrix[y].resize(width);
		for (int x = 0; x < width; ++x) {
			matrix[y][x] = new Empty(x, y);
		}
	}
}
	
CellularMatrix::~CellularMatrix() {
	// Clean up SDL resources
	if (renderTexture) {
		SDL_DestroyTexture(renderTexture);
	}
	
	// Clean up all elements in the grid
	for (int x = 0; x < WIDTH; ++x)
		for (int y = 0; y < HEIGHT; ++y)
			delete matrix[x][y];
}

bool CellularMatrix::step = false;

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
	// Bounds checking
	if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
		// Skip if element is already of desired type (optimization)
		if (matrix[y][x] && matrix[y][x]->getType() == type) {
			return;
		}
		
		// Replace existing element with new one
		Element* newElement = createElementFromType(type, x, y);
		delete matrix[y][x];
		matrix[y][x] = newElement;
	}
}

// In CellularMatrix.hpp and CellularMatrix.cpp:
void CellularMatrix::placeElementsInArea(int centerX, int centerY, int radius, ElementType type) {
    int r2 = radius * radius;
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

bool CellularMatrix::isInBounds(int x, int y) {
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

bool CellularMatrix::isEmpty(int x, int y) {
	return matrix[y][x]->getType() == EMPTY;
}

Element*& CellularMatrix::getElement(int x, int y) {
	return matrix[y][x];
}

//-------------------------------------------
// Simulation Update
//-------------------------------------------
void CellularMatrix::update() {
	static std::vector<int> columnOrder(WIDTH);
	// Initialize column indices (0 to WIDTH-1)
	std::iota(columnOrder.begin(), columnOrder.end(), 0);

	// Update from bottom to top (for gravity simulation)
	for (int y = HEIGHT - 1; y >= 0; --y) {
		// Randomize column update order to prevent bias
		std::shuffle(columnOrder.begin(), columnOrder.end(), rng);
		
		// Update each element in the randomized order
		for (int i = 0; i < WIDTH; ++i) {
			int x = columnOrder[i];
			matrix[y][x]->update(*this, x, y);
		}
	}
	step = !step;
}

std::vector<std::vector<Chunk>> CellularMatrix::generateChunks() {
	std::vector<std::vector<Chunk>> chunkVector{};
	int rows = static_cast<int>(std::ceil(static_cast<float>(HEIGHT / CHUNK_SIZE)));
	int cols = static_cast<int>(std::ceil(static_cast<float>(WIDTH / CHUNK_SIZE)));

	chunkVector.resize(rows);

	for (int row = 0; row < rows; ++row) {
		chunkVector[row].reserve(cols);
		for (int col = 0; col < cols; ++col) {
			int x = col * CHUNK_SIZE;
			int y = row * CHUNK_SIZE;

			Chunk newChunk(
				{std::min(x, WIDTH), std::min(y, HEIGHT)},
				{std::min(x + CHUNK_SIZE, WIDTH), std::min(y + CHUNK_SIZE, HEIGHT)}
			);

			chunkVector[col].push_back(newChunk);
		}
	}
	return chunkVector;
}

//-------------------------------------------
// Rendering
//-------------------------------------------
void CellularMatrix::render(SDL_Renderer* renderer) {
	// Convert element colors to pixel format
	for (int y = HEIGHT - 1; y >= 0; --y) {
		for (int x = 0; x < WIDTH; ++x) {
			SDL_Color color = matrix[y][x]->getColor();
			pixels[y * WIDTH + x] = (color.r << 24) | (color.g << 16) | 
								   (color.b << 8) | color.a;
		}
	}

	// Update and render texture
	SDL_UpdateTexture(renderTexture, NULL, pixels.data(), WIDTH * sizeof(Uint32));
	SDL_RenderCopy(renderer, renderTexture, NULL, NULL);

	// ✅ Draw grid if toggled
	if (showGrid) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100); // gray semi-transparent
		for (int x = 0; x < WIDTH; x += CHUNK_SIZE) {
			SDL_RenderDrawLine(renderer, x, 0, x, HEIGHT);
		}
		for (int y = HEIGHT - 1; y >= 0; y -= CHUNK_SIZE) {
			SDL_RenderDrawLine(renderer, 0, y, WIDTH, y);
		}
	}
}
