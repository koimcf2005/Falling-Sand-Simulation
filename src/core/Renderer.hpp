// src/core/Renderer.hpp
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include "src/core/CellularMatrix.hpp"
#include "src/ui/ElementUI.hpp"
#include "src/ui/DebugUI.hpp"

class Renderer {
public:
	Renderer(SDL_Renderer* sdlRenderer, int screenWidth, int screenHeight, int renderWidth, int renderHeight);

	void clear();
	void present();
	void setLogicalResolution();
	void resetLogicalResolution();
	void drawTexture(SDL_Texture* texture);
	void drawBrushOutline(int mouseX, int mouseY, int radius, bool mouseOverUI);
	void drawScreenSpaceRect(int x, int y, int width, int height, int thickness);

	void renderScene(CellularMatrix& matrix, ElementUI& ui, DebugUI& debug, bool showDebug);
	std::pair<int, int> windowToRenderCoords(int winX, int winY) const;
	std::pair<int, int> renderToWindowCoords(int renderX, int renderY) const;

private:
	SDL_Renderer* renderer;
	int screenWidth, screenHeight;
	int renderWidth, renderHeight;

	struct ScreenRect {
		int x, y, w, h, thickness;
	};
	std::vector<ScreenRect> queuedRects;
	void drawQueuedRects();

	void drawCircleOutline(int centerX, int centerY, int radius);
};

#endif // RENDERER_HPP