// src/core/Renderer.cpp
#include "Renderer.hpp"

// Global pointer to the main renderer instance
Renderer* g_Renderer = nullptr;

//------------------------------------------------------------------------------
// Renderer Constructor/Destructor and Initialization
//------------------------------------------------------------------------------

Renderer::Renderer()
	: mp_ElementUI(nullptr), mp_DebugUI(nullptr), mp_LightMap(nullptr)
{}

bool Renderer::initialize(const char* title) {
	// Initialize SDL and SDL_ttf
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init failed: " << SDL_GetError() << '\n';
		return false;
	}
	if (TTF_Init() == -1) {
		std::cerr << "TTF init failed: " << TTF_GetError() << '\n';
		return false;
	}

	// Create SDL window and renderer
	mp_Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window::WIDTH, Window::HEIGHT, SDL_WINDOW_SHOWN);
	if (!mp_Window) return false;
	mp_Renderer = SDL_CreateRenderer(mp_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mp_Renderer) return false;

	// Create light map texture and buffer
	mp_LightMap = SDL_CreateTexture(mp_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Matrix::WIDTH, Matrix::HEIGHT);
	m_LightBuffer.resize(Matrix::WIDTH * Matrix::HEIGHT * 3, 255); // Default: fully lit (white)

	// Construct UI overlays with the renderer
	mp_ElementUI = new ElementUI(mp_Renderer);
	mp_DebugUI = new DebugUI(mp_Renderer);

	// Initialize UI overlays
	if (!mp_ElementUI->initialize()) {
		std::cerr << "Failed to initialize Element UI\n";
		cleanup();
		return false;
	}
	if (!mp_DebugUI->initialize()) {
		std::cerr << "Failed to initialize Debug UI\n";
		cleanup();
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
// Rendering and Presentation
//------------------------------------------------------------------------------

void Renderer::clear() {
	// Clear the screen to black
	SDL_SetRenderDrawColor(mp_Renderer, 0, 0, 0, 255);
	SDL_RenderClear(mp_Renderer);
}

void Renderer::present() {
	// Present the rendered frame to the window
	SDL_RenderPresent(mp_Renderer);
}

void Renderer::setLogicalResolution() {
	// Set logical rendering size to simulation grid size
	SDL_RenderSetLogicalSize(mp_Renderer, Matrix::WIDTH, Matrix::HEIGHT);
}

void Renderer::resetLogicalResolution() {
	// Reset logical rendering size to window size (full resolution)
	SDL_RenderSetLogicalSize(mp_Renderer, 0, 0);
}

void Renderer::renderScene(CellularMatrix& matrix, bool showDebug) {
	// Render the simulation scene, UI, and debug overlays
	clear();

	// Draw low-res game world
	matrix.updateTexture();
	drawTexture(matrix.getTexture());

	// Switch to full-res and render overlays
	resetLogicalResolution();
	mp_ElementUI->render();
	mp_DebugUI->render(showDebug);

	drawQueuedRects();

	setLogicalResolution(); // Restore for next frame
}

void Renderer::drawTexture(SDL_Texture* texture) {
	// Draw a texture to the renderer
	SDL_RenderCopy(mp_Renderer, texture, nullptr, nullptr);
}

//------------------------------------------------------------------------------
// Drawing Utilities
//------------------------------------------------------------------------------

void Renderer::drawBrushOutline(int x, int y, int radius, bool mouseOverUI) {
	// Draw a circular outline for the brush if not over UI and within bounds
	if (mouseOverUI || x < 0 || x >= Matrix::WIDTH || y < 0 || y >= Matrix::HEIGHT) return;
	SDL_SetRenderDrawColor(mp_Renderer, 255, 255, 255, 255);
	drawCircleOutline(x, y, radius);
}

void Renderer::drawCircleOutline(int centerX, int centerY, int radius) {
	// Draw a circle outline using the midpoint circle algorithm
	const int diameter = radius * 2;
	int x = radius - 1, y = 0;
	int tx = 1, ty = 1;
	int err = tx - diameter;

	while (x >= y) {
		SDL_RenderDrawPoint(mp_Renderer, centerX + x, centerY - y);
		SDL_RenderDrawPoint(mp_Renderer, centerX + x, centerY + y);
		SDL_RenderDrawPoint(mp_Renderer, centerX - x, centerY - y);
		SDL_RenderDrawPoint(mp_Renderer, centerX - x, centerY + y);
		SDL_RenderDrawPoint(mp_Renderer, centerX + y, centerY - x);
		SDL_RenderDrawPoint(mp_Renderer, centerX + y, centerY + x);
		SDL_RenderDrawPoint(mp_Renderer, centerX - y, centerY - x);
		SDL_RenderDrawPoint(mp_Renderer, centerX - y, centerY + x);

		if (err <= 0) {
			y++;
			err += ty;
			ty += 2;
		}
		if (err > 0) {
			x--;
			tx += 2;
			err += tx - diameter;
		}
	}
}

void Renderer::drawScreenSpaceRect(int x, int y, int width, int height, int thickness) {
	// Queue a rectangle in screen (window) space for drawing
	auto [winX, winY] = renderToWindowCoords(x, y);
	auto [winX2, winY2] = renderToWindowCoords(x + width, y + height);

	int screenWidth = winX2 - winX;
	int screenHeight = winY2 - winY;

	m_QueuedRects.push_back({ winX, winY, screenWidth, screenHeight, thickness });
}

void Renderer::drawQueuedRects() {
	// Draw all queued screen-space rectangles (used for overlays)
	resetLogicalResolution();
	SDL_SetRenderDrawColor(mp_Renderer, 255, 0, 0, 255); // Red outlines

	for (const auto& rect : m_QueuedRects) {
		SDL_Rect top    = { rect.x, rect.y, rect.w, rect.thickness };
		SDL_Rect bottom = { rect.x, rect.y + rect.h - rect.thickness, rect.w, rect.thickness };
		SDL_Rect left   = { rect.x, rect.y, rect.thickness, rect.h };
		SDL_Rect right  = { rect.x + rect.w - rect.thickness, rect.y, rect.thickness, rect.h };

		SDL_RenderFillRect(mp_Renderer, &top);
		SDL_RenderFillRect(mp_Renderer, &bottom);
		SDL_RenderFillRect(mp_Renderer, &left);
		SDL_RenderFillRect(mp_Renderer, &right);
	}

	m_QueuedRects.clear();
	setLogicalResolution();
}

//------------------------------------------------------------------------------
// Coordinate Conversion
//------------------------------------------------------------------------------

std::pair<int, int> Renderer::windowToRenderCoords(int winX, int winY) const {
	// Convert window (screen) coordinates to simulation (render) coordinates
	int renderX = winX * Matrix::WIDTH / Window::WIDTH;
	int renderY = winY * Matrix::HEIGHT / Window::HEIGHT;
	return { renderX, renderY };
}

std::pair<int, int> Renderer::renderToWindowCoords(int renderX, int renderY) const {
	// Convert simulation (render) coordinates to window (screen) coordinates
	int winX = renderX * Window::WIDTH / Matrix::WIDTH;
	int winY = renderY * Window::HEIGHT / Matrix::HEIGHT;
	return { winX, winY };
}

//------------------------------------------------------------------------------
// Accessors
//------------------------------------------------------------------------------

SDL_Renderer* Renderer::getRenderer() {
	return mp_Renderer;
}

ElementUI* Renderer::getElementUI() {
	return mp_ElementUI;
}

DebugUI* Renderer::getDebugUI() {
	return mp_DebugUI;
}

//------------------------------------------------------------------------------
// Cleanup
//------------------------------------------------------------------------------

void Renderer::cleanup() {
	// Destroy SDL resources and UI overlays
	if (mp_Renderer) SDL_DestroyRenderer(mp_Renderer);
	if (mp_Window) SDL_DestroyWindow(mp_Window);
	if (mp_ElementUI) {
		mp_ElementUI->cleanup();
		delete mp_ElementUI;
		mp_ElementUI = nullptr;
	}
	if (mp_DebugUI) {
		delete mp_DebugUI;
		mp_DebugUI = nullptr;
	}
	if (mp_LightMap) {
		SDL_DestroyTexture(mp_LightMap);
		mp_LightMap = nullptr;
	}
	SDL_Quit();
}