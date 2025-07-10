#include "src/elements/solids/immoveable/ImmoveableSolid.hpp"

/// ==================== Update ====================
void ImmoveableSolid::update(IMatrix& matrix, int x, int y) {
	handleHeat(matrix, x, y);
}