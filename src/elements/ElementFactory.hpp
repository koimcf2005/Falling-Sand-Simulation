#ifndef ELEMENT_FACTORY_HPP
#define ELEMENT_FACTORY_HPP

#include <map>
#include <random>
#include <algorithm>
#include <SDL2/SDL.h>

//-------------------------------------------
// Element Types
//-------------------------------------------
enum ElementType {
	EMPTY,
	SAND,
	DIRT,
	COAL,
	SALT,
	STONE,
	WOOD,
	WATER,
	OIL,
	SMOKE,
	STEAM
};

// Forward declare Element class since we only need the pointer type
class Element;

//-------------------------------------------
// Factory Function Declaration
//-------------------------------------------
Element* createElementFromType(ElementType type, int x, int y);

class ElementFactory {
	public:
		// All methods and members are static
		static SDL_Color getColorByElementType(ElementType type, int x, int y);
		static void initialize();
		
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

#endif // ELEMENT_factory_HPP