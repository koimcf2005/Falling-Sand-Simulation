#include "src/elements/ElementUtilities.hpp"
#include "src/elements/ElementTypes.hpp"
#include "src/matrix/Matrix.hpp"

bool ElementUtilities::canSwapWithElement(const Matrix& matrix, const int x, const int y) {
	if (!matrix.isInBounds(x, y)) return false;
	if (matrix.getElement(x, y).type != EMPTY) return false;
	return true;
}