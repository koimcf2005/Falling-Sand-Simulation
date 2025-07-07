#include "ElementFactory.hpp"
#include <SDL_image.h>
#include <iostream>

#include "solids/movable/Sand.hpp"
#include "solids/movable/Dirt.hpp"
#include "solids/movable/Coal.hpp"
#include "solids/movable/Salt.hpp"
#include "solids/immoveable/Stone.hpp"
#include "solids/immoveable/Wood.hpp"
#include "liquids/Water.hpp"
#include "liquids/Oil.hpp"
#include "gases/Smoke.hpp"
#include "gases/Steam.hpp"
#include "Empty.hpp"

//-------------------------------------------
// Factory Function Implementation
//-------------------------------------------
Element* createElementFromType(ElementType type, int x, int y) {
	switch (type) {
		case SAND: return new Sand(x, y);
		case DIRT: return new Dirt(x, y);
		case COAL: return new Coal(x, y);
		case SALT: return new Salt(x, y);
		case STONE: return new Stone(x, y);
		case WOOD: return new Wood(x, y);
		case WATER: return new Water(x, y);
		case OIL: return new Oil(x, y);
		case SMOKE: return new Smoke(x, y);
		case STEAM: return new Steam(x, y);
		default: return new Empty(x, y);
	}
}

const ElementFactory::ElementColor ElementFactory::C_EMPTY { SDL_Color {0, 0, 0, 0}, 0 };
const ElementFactory::ElementColor ElementFactory::C_SAND { SDL_Color {194, 178, 128, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_DIRT { SDL_Color {98, 50, 19, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_COAL { SDL_Color {35, 35, 35, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_SALT { SDL_Color {255, 255, 255, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_WATER { SDL_Color {51, 82, 72, 100}, 1 };
const ElementFactory::ElementColor ElementFactory::C_OIL { SDL_Color {40, 40, 40, 100}, 1 };
const ElementFactory::ElementColor ElementFactory::C_STONE { SDL_Color {128, 128, 128, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_WOOD { SDL_Color {134, 97, 45, 255}, 5 };
const ElementFactory::ElementColor ElementFactory::C_SMOKE { SDL_Color {33, 33, 33, 125}, 1 };
const ElementFactory::ElementColor ElementFactory::C_STEAM { SDL_Color {100, 100, 100, 125}, 1 };

std::map<ElementType, ElementFactory::ElementColor> ElementFactory::colorMap;
std::map<ElementType, SDL_Surface*> ElementFactory::textureMap;

std::mt19937 ElementFactory::rng{std::random_device{}()};

void ElementFactory::initialize() {
	colorMap[EMPTY] = C_EMPTY;
	colorMap[SAND] = C_SAND;
	colorMap[DIRT] = C_DIRT;
	colorMap[COAL] = C_COAL;
	colorMap[SALT] = C_SALT;
	colorMap[WATER] = C_WATER;
	colorMap[OIL] = C_OIL;
	colorMap[STONE] = C_STONE;
	colorMap[WOOD] = C_WOOD;
	colorMap[SMOKE] = C_SMOKE;
	colorMap[STEAM] = C_STEAM;

	SDL_Surface* stone_texture = IMG_Load("assets/textures/stone.png");
	if (stone_texture) textureMap[STONE] = stone_texture;
	SDL_Surface* wood_texture = IMG_Load("assets/textures/wood.png");
	if (wood_texture) textureMap[WOOD] = wood_texture;
}

SDL_Color ElementFactory::getColorByElementType(ElementType type, int x, int y) {
	auto it = textureMap.find(type);
	if (it == textureMap.end() || !it->second) {
		return getOffsetColor(type);
	}
	else {
		return getTextureColor(type, it->second, x, y);
	}
}

SDL_Color ElementFactory::getTextureColor(ElementType /*type*/, SDL_Surface* surface, int x, int y) {
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

SDL_Color ElementFactory::getOffsetColor(ElementType type) {
	SDL_Color color = colorMap[type].color;
	int offset = colorMap[type].offset;
	int newOffset = getRandomOffset(offset);
	color.r = std::clamp(color.r + newOffset, 0, 255);
	color.g = std::clamp(color.g + newOffset, 0, 255);
	color.b = std::clamp(color.b + newOffset, 0, 255);
	color.a = 255;
	return color;
}

int ElementFactory::getRandomOffset(int offset) {
	std::uniform_int_distribution<int> dist(-offset, offset);
	return dist(rng);
}