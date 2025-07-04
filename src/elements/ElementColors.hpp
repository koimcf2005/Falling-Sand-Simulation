#ifndef ELEMENT_COLORS_HPP
#define ELEMENT_COLORS_HPP

#include <map>
#include <random>
#include <algorithm>
#include <SDL2/SDL.h>
#include "ElementType.hpp"

class ElementColors {
	public:
		// All methods and members are static
		static SDL_Color getColorByElementType(ElementType type, int x, int y);
		static void initialize(SDL_Renderer* renderer);
		
	private:
		struct ElementColor {
			SDL_Color color;
			int offset;
			ElementColor() : color{0, 0, 0, 0}, offset(0) {}
			ElementColor(const SDL_Color& c, int o)
			: color(c), offset(o)
			{}
		};
		
		static SDL_Color getTextureColor(ElementType type, SDL_Surface* surface, int x, int y);
		static SDL_Color getOffsetColor(ElementType type);
		
		static std::map<ElementType, ElementColor> colorMap;
		static std::map<ElementType, SDL_Surface*> textureMap;
		static std::mt19937 rng;
		static int getRandomOffset(int offset);

		static const ElementColor C_EMPTY;
		static const ElementColor C_SAND;
		static const ElementColor C_DIRT;
		static const ElementColor C_COAL;
		static const ElementColor C_SALT;
		static const ElementColor C_WATER;
		static const ElementColor C_OIL;
		static const ElementColor C_STONE;
		static const ElementColor C_WOOD;
		static const ElementColor C_SMOKE;
		static const ElementColor C_STEAM;
};

#endif