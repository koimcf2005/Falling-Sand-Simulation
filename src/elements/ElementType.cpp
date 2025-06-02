#include "ElementType.hpp"
#include "solids/movable/Sand.hpp"
#include "solids/movable/Dirt.hpp"
#include "solids/movable/Coal.hpp"
#include "solids/movable/Salt.hpp"
#include "solids/immoveable/Stone.hpp"
#include "solids/immoveable/Wood.hpp"
#include "liquids/Water.hpp"
#include "liquids/Oil.hpp"
#include "gases/Smoke.hpp"
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
        default: return new Empty(x, y);
    }
}
