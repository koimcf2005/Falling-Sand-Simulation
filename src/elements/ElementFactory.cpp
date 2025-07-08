// src/elements/ElementFactory.cpp

#include "ElementFactory.hpp"
#include <SDL_image.h>
#include <iostream>

// Element class includes
#include "src/elements/solids/movable/Sand.hpp"
#include "src/elements/solids/movable/Dirt.hpp"
#include "src/elements/solids/movable/Coal.hpp"
#include "src/elements/solids/movable/Salt.hpp"
#include "src/elements/solids/immoveable/Stone.hpp"
#include "src/elements/solids/immoveable/Wood.hpp"
#include "src/elements/liquids/Water.hpp"
#include "src/elements/liquids/Oil.hpp"
#include "src/elements/gases/Smoke.hpp"
#include "src/elements/gases/Steam.hpp"
#include "src/elements/EmptyElement.hpp"

// ===============================
// Factory Function Implementation
// ===============================

/**
 * Creates a new instance of an Element based on its type.
 * If the element type is registered and has a factory function, use it.
 * Otherwise, return a default Empty element.
 */
Element* ElementFactory::createElementFromType(ElementType type, int x, int y) {
	auto it = elementRegistry.find(type);
	if (it != elementRegistry.end() && it->second.factory) {
		return it->second.factory(x, y);
	}
	return new Empty(x, y);
}

// Static member definitions
std::map<ElementType, ElementFactory::ElementInfo> ElementFactory::elementRegistry;
std::map<ElementType, SDL_Surface*> ElementFactory::textureMap;
std::vector<ElementType> ElementFactory::registeredElements;
std::mt19937 ElementFactory::rng{std::random_device{}()};

/**
 * Template method to register a new element in the registry.
 * Stores metadata, a factory function, and optional texture path.
 */
template<typename T>
void ElementFactory::registerElement(ElementType type, const std::string& name,
									 const SDL_Color& color, int colorOffset,
									 const std::string& texturePath) {
	elementRegistry[type] = ElementInfo(
		name,
		color,
		colorOffset,
		[](int x, int y) -> Element* { return new T(x, y); },
		texturePath
	);
	registeredElements.push_back(type);
}

/**
 * Initializes the element registry with all known types.
 * Also loads textures for any elements that specify a texture path.
 */
void ElementFactory::initialize() {
	elementRegistry.clear();
	registeredElements.clear();

	// Register all element types here (type, name, base color, color offset, [optional] texture path)
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

	// Load textures for elements that have a valid texture path
	for (const auto& [type, info] : elementRegistry) {
		if (!info.texturePath.empty()) {
			SDL_Surface* texture = IMG_Load(info.texturePath.c_str());
			if (texture) {
				textureMap[type] = texture;
			} else {
				std::cerr << "Failed to load texture for " << info.name << ": " << info.texturePath << '\n';
			}
		}
	}
}

/**
 * Returns a list of all registered ElementTypes.
 */
std::vector<ElementType> ElementFactory::getRegisteredElements() {
	return registeredElements;
}

/**
 * Returns the name string associated with an ElementType.
 */
std::string ElementFactory::getElementName(ElementType type) {
	auto it = elementRegistry.find(type);
	return (it != elementRegistry.end()) ? it->second.name : "Unknown";
}

/**
 * Determines the color of an element at (x, y), using either a texture
 * (if available) or a base color with offset variation.
 */
SDL_Color ElementFactory::getColorByElementType(ElementType type, int x, int y) {
	auto it = textureMap.find(type);
	if (it == textureMap.end() || !it->second) {
		return getOffsetColor(type);
	} else {
		return getTextureColor(type, it->second, x, y);
	}
}

/**
 * Samples a pixel color from a texture at coordinates (x, y), wrapping
 * coordinates around the texture's width and height.
 */
SDL_Color ElementFactory::getTextureColor(ElementType /*type*/, SDL_Surface* surface, int x, int y) {
	x = ((x % surface->w) + surface->w) % surface->w;  // wrap around texture width
	y = ((y % surface->h) + surface->h) % surface->h;  // wrap around texture height

	Uint8* pixel_ptr = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * surface->format->BytesPerPixel;
	Uint32 pixel = 0;
	memcpy(&pixel, pixel_ptr, surface->format->BytesPerPixel);

	SDL_Color color;
	SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

/**
 * Returns a color based on the base color of the element,
 * with a slight random offset applied for visual variation.
 */
SDL_Color ElementFactory::getOffsetColor(ElementType type) {
	auto it = elementRegistry.find(type);
	if (it == elementRegistry.end()) {
		return {0, 0, 0, 255};  // fallback color
	}

	SDL_Color color = it->second.color;
	int offset = getRandomOffset(it->second.colorOffset);
	color.r = std::clamp(color.r + offset, 0, 255);
	color.g = std::clamp(color.g + offset, 0, 255);
	color.b = std::clamp(color.b + offset, 0, 255);
	color.a = 255;
	return color;
}

/**
 * Returns a random integer between [-offset, offset].
 */
int ElementFactory::getRandomOffset(int offset) {
	std::uniform_int_distribution<int> dist(-offset, offset);
	return dist(rng);
}