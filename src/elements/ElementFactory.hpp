// src/elements/ElementFactory.hpp
#ifndef ELEMENT_FACTORY_HPP
#define ELEMENT_FACTORY_HPP

#include <map>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
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
	ASH,
	STONE,
	WOOD,
	WATER,
	OIL,
	SMOKE,
	STEAM,
	FIRE,
};

// Forward declare Element class since we only need the pointer type
class Element;

class ElementFactory {
	public:
		// All methods and members are static
		static SDL_Color getColorByElementType(ElementType type, int x, int y);
		static void initialize();
		
		// New registration system
		static std::vector<ElementType> getRegisteredElements();
		static std::string getElementName(ElementType type);
		static Element* createElementFromType(ElementType type, int x, int y);
		
	private:
		struct ElementInfo {
			std::string name;
			SDL_Color color;
			int colorOffset;
			std::function<Element*(int, int)> factory;
			std::string texturePath;
			
			ElementInfo()
				: name(""),
				color{0, 0, 0, 0},
				colorOffset(0),
				texturePath("")
			{}

			ElementInfo(const std::string& n, const SDL_Color& c, int offset,
						std::function<Element*(int, int)> f, const std::string& texture = "")
				: name(n),
				color(c),
				colorOffset(offset),
				factory(f),
				texturePath(texture)
			{}
		};
		
		static SDL_Color getTextureColor(ElementType type, SDL_Surface* surface, int x, int y);
		static SDL_Color getOffsetColor(ElementType type);
		
		static std::map<ElementType, ElementInfo> elementRegistry;
		static std::map<ElementType, SDL_Surface*> textureMap;
		static std::vector<ElementType> registeredElements;
		static std::mt19937 rng;
		static int getRandomOffset(int offset);
		
		// Registration helper
		template<typename T>
		static void registerElement(ElementType type, const std::string& name, 
									const SDL_Color& color, int colorOffset,
									const std::string& texturePath = "");
};

#endif // ELEMENT_FACTORY_HPP