#include "src/elements/ElementFactory.hpp"
#include "src/elements/types/Empty/EmptyElement.hpp"
#include "src/elements/types/Sand/SandElement.hpp"
#include "src/matrix/Matrix.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>

std::array<ElementFactory::ElementInfo, ElementType_Count> ElementFactory::m_element_registry;
std::array<SDL_Surface*, ElementType_Count> ElementFactory::m_texture_registry;
std::mt19937 ElementFactory::m_rng{std::random_device{}()};

void ElementFactory::initialize() {
	// Register all element types here (type, name, base color, color offset, [optional] texture path)
	registerElement(EMPTY, "Empty", {0, 0, 0, 0}, 0);
	registerElement(SAND, "Sand", {194, 178, 128, 255}, 5);
	// registerElement<Dirt>(DIRT, "Dirt", {98, 50, 19, 255}, 5);
	// registerElement<Coal>(COAL, "Coal", {35, 35, 35, 255}, 5);
	// registerElement<Salt>(SALT, "Salt", {255, 255, 255, 255}, 5);
	// registerElement<Ash>(ASH, "Ash", {143, 143, 143, 255}, 15);
	// registerElement<Water>(WATER, "Water", {51, 82, 72, 100}, 1);
	// registerElement<Oil>(OIL, "Oil", {40, 40, 40, 100}, 1);
	// registerElement<Stone>(STONE, "Stone", {128, 128, 128, 255}, 5, "assets/textures/stone.png");
	// registerElement<Wood>(WOOD, "Wood", {134, 97, 45, 255}, 5, "assets/textures/wood.png");
	// registerElement<Smoke>(SMOKE, "Smoke", {33, 33, 33, 125}, 1);
	// registerElement<Steam>(STEAM, "Steam", {100, 100, 100, 125}, 1);
	// registerElement<Fire>(FIRE, "Fire", {255, 165, 0, 200}, 10);
}

void ElementFactory::initializeElementByType(ElementType type, entt::entity entity) {
	switch (type) {
		case EMPTY: Empty::initialize(entity); break;
		case SAND: Sand::initialize(entity); break;
		default: break;
	}
}

void ElementFactory::updateElementByType(ElementType type, Matrix& matrix, entt::entity entity) {
	switch (type) {
		case EMPTY: Empty::update(matrix, entity); break;
		case SAND: Sand::update(matrix, entity); break;
		default: break;
	}
}

entt::entity ElementFactory::createElementByType(const ElementType type,
																									 const int x, const int y) {
	entt::entity newEntity = ComponentManager::createEntity();
	SDL_Color color = getElementColor(type, x, y);
	ComponentManager::addComponent<Element>(
		newEntity,
		Element{
			type,
			color,
			color,
			!Matrix::getStep()
		}
	);

	ComponentManager::addComponent<Position>(newEntity, Position(x, y));

	initializeElementByType(type, newEntity);
	
	return newEntity;
}

SDL_Color ElementFactory::getElementColor(const ElementType type, const int x, const int y) {
	SDL_Surface* texture = m_texture_registry[type];
	if (!texture) {
		return getOffsetColor(type);
	} else {
		return getTextureColor(texture, x, y);
	}
}

std::string ElementFactory::getElementName(ElementType type) {
	return m_element_registry[type].name;
}

SDL_Color ElementFactory::getTextureColor(const SDL_Surface* surface, const int x, const int y) {
	int tx = ((x % surface->w) + surface->w) % surface->w;
	int ty = ((y % surface->h) + surface->h) % surface->h;

	Uint8* pixel_ptr = static_cast<Uint8*>(surface->pixels) + ty *
										 surface->pitch + tx * surface->format->BytesPerPixel;
	Uint32 pixel = 0;
	memcpy(&pixel, pixel_ptr, surface->format->BytesPerPixel);

	SDL_Color color;
	SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

SDL_Color ElementFactory::getOffsetColor(const ElementType type) {
	SDL_Color color = m_element_registry[type].color;
	int offset = getRandomOffset(m_element_registry[type].colorOffset);
	color.r = std::clamp(color.r + offset, 0, 255);
	color.g = std::clamp(color.g + offset, 0, 255);
	color.b = std::clamp(color.b + offset, 0, 255);
	return color;
}

int ElementFactory::getRandomOffset(const int offset) {
	std::uniform_int_distribution<int> dist(-offset, offset);
	return dist(m_rng);
}

void ElementFactory::registerElement(const ElementType type, const std::string& name,
																		 const SDL_Color& color, const int colorOffset,
																		 const std::string& texturePath) {
	std::cout << "registerElement: type=" << type << ", name=" << name << ", colorOffset=" << colorOffset << ", texturePath='" << texturePath << "'" << std::endl;
	m_element_registry[type] = ElementInfo(
		name,
		color,
		colorOffset,
		texturePath
	);

	if (!texturePath.empty()) {
		std::cout << "Attempting to load texture: " << texturePath << std::endl;
		SDL_Surface* surface = IMG_Load(texturePath.c_str());
		if (surface) {
			std::cout << "Texture loaded successfully for " << name << std::endl;
			m_texture_registry[type] = surface;
		} else {
			std::cerr << "Failed to load texture for " << name << ": " << texturePath << '\n';
		}
	}
}

void ElementFactory::cleanup() {
	for (auto& surface : m_texture_registry) {
		if (surface) {
			SDL_FreeSurface(surface);
			surface = nullptr;
		}
	}
}