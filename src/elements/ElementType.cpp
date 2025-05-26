#include "ElementType.hpp"
#include "solids/movable/Sand.hpp"
#include "solids/immoveable/Stone.hpp"
#include "gases/Smoke.hpp"
#include "Empty.hpp"

//-------------------------------------------
// Factory Function Implementation
//-------------------------------------------
Element* createElementFromType(ElementType type, int x, int y) {
    switch (type) {
        case SAND: return new Sand(x, y);
        case STONE: return new Stone(x, y);
		case SMOKE: return new Smoke(x, y);
        default: return new Empty(x, y);
    }
}
