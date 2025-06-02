#ifndef ELEMENT_TYPE_HPP
#define ELEMENT_TYPE_HPP

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
	NOT_MOVING
};

// Forward declare Element class since we only need the pointer type
class Element;

//-------------------------------------------
// Factory Function Declaration
//-------------------------------------------
Element* createElementFromType(ElementType type, int x, int y);

#endif // ELEMENT_TYPE_HPP
