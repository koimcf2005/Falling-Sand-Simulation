#include "ElementColors.hpp"
#include <SDL_image.h>
#include <iostream>

const ElementColors::ElementColor ElementColors::C_EMPTY { SDL_Color {0, 0, 0, 0}, 0 };
const ElementColors::ElementColor ElementColors::C_SAND { SDL_Color {194, 178, 128, 255}, 5 };
const ElementColors::ElementColor ElementColors::C_DIRT { SDL_Color {98, 50, 19, 255}, 5 };
const ElementColors::ElementColor ElementColors::C_COAL { SDL_Color {35, 35, 35, 255}, 5 };
const ElementColors::ElementColor ElementColors::C_WATER { SDL_Color {51, 82, 72, 100}, 1 };
const ElementColors::ElementColor ElementColors::C_STONE { SDL_Color {128, 128, 128, 255}, 5 };
const ElementColors::ElementColor ElementColors::C_WOOD { SDL_Color {134, 97, 45, 255}, 5 };
const ElementColors::ElementColor ElementColors::C_SMOKE { SDL_Color {33, 33, 33, 125}, 1 };

std::map<ElementType, ElementColors::ElementColor> ElementColors::colorMap;
std::map<ElementType, SDL_Surface*> ElementColors::textureMap;

std::mt19937 ElementColors::rng{std::random_device{}()};

void ElementColors::initialize(SDL_Renderer* renderer) {
	colorMap[EMPTY] = C_EMPTY;
	colorMap[SAND] = C_SAND;
	colorMap[DIRT] = C_DIRT;
	colorMap[COAL] = C_COAL;
	colorMap[WATER] = C_WATER;
	colorMap[STONE] = C_STONE;
	colorMap[WOOD] = C_WOOD;
	colorMap[SMOKE] = C_SMOKE;

	SDL_Surface* stone_texture = IMG_Load("src/elements/textures/stone.png");
	if (stone_texture) textureMap[STONE] = stone_texture;
	SDL_Surface* wood_texture = IMG_Load("src/elements/textures/wood.png");
	if (wood_texture) textureMap[WOOD] = wood_texture;
}

SDL_Color ElementColors::getColorByElementType(ElementType type, int x, int y) {
	auto it = textureMap.find(type);
	if (it == textureMap.end() || !it->second) {
		return getOffsetColor(type);
	}
	else {
		return getTextureColor(type, it->second, x, y);
	}
}

SDL_Color ElementColors::getTextureColor(ElementType type, SDL_Surface* surface, int x, int y) {
	// Tile the texture
	x = ((x % surface->w) + surface->w) % surface->w;
	y = ((y % surface->h) + surface->h) % surface->h;
	Uint8* pixel_ptr = (Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
	Uint32 pixel = 0;
	memcpy(&pixel, pixel_ptr, surface->format->BytesPerPixel);
	SDL_Color color;
	SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

SDL_Color ElementColors::getOffsetColor(ElementType type) {
	SDL_Color color = colorMap[type].color;
	int offset = colorMap[type].offset;
	int newOffset = getRandomOffset(offset);
	color.r = std::clamp(color.r + newOffset, 0, 255);
	color.g = std::clamp(color.g + newOffset, 0, 255);
	color.b = std::clamp(color.b + newOffset, 0, 255);
	color.a = 255;
	return color;
}

int ElementColors::getRandomOffset(int offset) {
	std::uniform_int_distribution<int> dist(-offset, offset);
	return dist(rng);
}