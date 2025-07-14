// src/render/Renderer.cpp
#include "Renderer.hpp"

Renderer* gRenderer = nullptr;  // Definition of the global pointer

Renderer::Renderer(SDL_Renderer* sdlRenderer, int screenW, int screenH, int renderW, int renderH)
	: renderer(sdlRenderer), screenWidth(screenW), screenHeight(screenH),
	  renderWidth(renderW), renderHeight(renderH) {}

void Renderer::clear() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
}

void Renderer::present() {
	SDL_RenderPresent(renderer);
}

void Renderer::setLogicalResolution() {
	SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);
}

void Renderer::resetLogicalResolution() {
	SDL_RenderSetLogicalSize(renderer, 0, 0); // full resolution
}

void Renderer::renderScene(CellularMatrix& matrix, ElementUI& ui, DebugUI& debug, bool showDebug) {
	clear();

	// Draw low-res game world
	matrix.updateTexture();
	drawTexture(matrix.getTexture());
	ui.render(renderer);

	// Switch to full-res and render debug overlay
	resetLogicalResolution();
	debug.render(showDebug);

	drawQueuedRects();

	setLogicalResolution(); // Restore for next frame
}

void Renderer::drawTexture(SDL_Texture* texture) {
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

void Renderer::drawBrushOutline(int x, int y, int radius, bool mouseOverUI) {
	if (mouseOverUI || x < 0 || x >= renderWidth || y < 0 || y >= renderHeight) return;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawCircleOutline(x, y, radius);
}

void Renderer::drawCircleOutline(int centerX, int centerY, int radius) {
	const int diameter = radius * 2;
	int x = radius - 1, y = 0;
	int tx = 1, ty = 1;
	int err = tx - diameter;

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
		SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
		SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
		SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
		SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
		SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
		SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
		SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);

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
	// Convert top-left corner and size from logical (render) to screen (window) coordinates
	auto [winX, winY] = renderToWindowCoords(x, y);
	auto [winX2, winY2] = renderToWindowCoords(x + width, y + height);

	int screenWidth = winX2 - winX;
	int screenHeight = winY2 - winY;

	queuedRects.push_back({ winX, winY, screenWidth, screenHeight, thickness });
}

void Renderer::drawQueuedRects() {
	resetLogicalResolution();
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red outlines

	for (const auto& rect : queuedRects) {
		SDL_Rect top    = { rect.x, rect.y, rect.w, rect.thickness };
		SDL_Rect bottom = { rect.x, rect.y + rect.h - rect.thickness, rect.w, rect.thickness };
		SDL_Rect left   = { rect.x, rect.y, rect.thickness, rect.h };
		SDL_Rect right  = { rect.x + rect.w - rect.thickness, rect.y, rect.thickness, rect.h };

		SDL_RenderFillRect(renderer, &top);
		SDL_RenderFillRect(renderer, &bottom);
		SDL_RenderFillRect(renderer, &left);
		SDL_RenderFillRect(renderer, &right);
	}

	queuedRects.clear();
	setLogicalResolution();
}

std::pair<int, int> Renderer::windowToRenderCoords(int winX, int winY) const {
	int renderX = winX * renderWidth / screenWidth;
	int renderY = winY * renderHeight / screenHeight;
	return { renderX, renderY };
}

std::pair<int, int> Renderer::renderToWindowCoords(int renderX, int renderY) const {
	int winX = renderX * screenWidth / renderWidth;
	int winY = renderY * screenHeight / renderHeight;
	return { winX, winY };
}
