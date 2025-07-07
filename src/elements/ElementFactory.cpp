// src/elements/ElementFactory.cpp
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
Element* ElementFactory::createElementFromType(ElementType type, int x, int y) {
	auto it = elementRegistry.find(type);
	if (it != elementRegistry.end() && it->second.factory) {
		return it->second.factory(x, y);
	}
	return new Empty(x, y);
}

std::map<ElementType, ElementFactory::ElementInfo> ElementFactory::elementRegistry;
std::map<ElementType, SDL_Surface*> ElementFactory::textureMap;
std::vector<ElementType> ElementFactory::registeredElements;
std::mt19937 ElementFactory::rng{std::random_device{}()};

template<typename T>
void ElementFactory::registerElement(ElementType type, const std::string& name, 
									const SDL_Color& color, int colorOffset,
									const std::string& texturePath) {
	elementRegistry[type] = ElementInfo(
		name, color, colorOffset,
		[](int x, int y) -> Element* { return new T(x, y); },
		texturePath
	);
	registeredElements.push_back(type);
}

void ElementFactory::initialize() {
	// Clear existing registrations
	elementRegistry.clear();
	registeredElements.clear();
	
	// Register all elements in one place
	registerElement<Empty>(EMPTY, "Empty", {0, 0, 0, 0}, 0);
	registerElement<Sand>(SAND, "Sand", {194, 178, 128, 255}, 5);
	registerElement<Dirt>(DIRT, "Dirt", {98, 50, 19, 255}, 5);
	registerElement<Coal>(COAL, "Coal", {35, 35, 35, 255}, 5);
	registerElement<Salt>(SALT, "Salt", {255, 255, 255, 255}, 5);
	registerElement<Water>(WATER, "Water", {51, 82, 72, 100}, 1);
	registerElement<Oil>(OIL, "Oil", {40, 40, 40, 100}, 1);
	registerElement<Stone>(STONE, "Stone", {128, 128, 128, 255}, 5, "assets/textures/stone.png");
	registerElement<Wood>(WOOD, "Wood", {134, 97, 45, 255}, 5, "assets/textures/wood.png");
	registerElement<Smoke>(SMOKE, "Smoke", {33, 33, 33, 125}, 1);
	registerElement<Steam>(STEAM, "Steam", {100, 100, 100, 125}, 1);

	// Load textures for elements that have them
	for (const auto& pair : elementRegistry) {
		if (!pair.second.texturePath.empty()) {
			SDL_Surface* texture = IMG_Load(pair.second.texturePath.c_str());
			if (texture) {
				textureMap[pair.first] = texture;
			}
		}
	}
}

std::vector<ElementType> ElementFactory::getRegisteredElements() {
	return registeredElements;
}

std::string ElementFactory::getElementName(ElementType type) {
	auto it = elementRegistry.find(type);
	if (it != elementRegistry.end()) {
		return it->second.name;
	}
	return "Unknown";
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
	auto it = elementRegistry.find(type);
	if (it == elementRegistry.end()) {
		return {0, 0, 0, 255};
	}
	
	SDL_Color color = it->second.color;
	int offset = it->second.colorOffset;
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