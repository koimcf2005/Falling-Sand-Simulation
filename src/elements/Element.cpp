// -------------------------------------------
// Element.cpp
// -------------------------------------------
// Implementation of the base Element class.
// Handles construction, color, step flag, and utility functions.
// -------------------------------------------
#include "Element.hpp"
#include "CellularMatrix.hpp"

// --- Static RNG initialization ---
std::mt19937 Element::rng{std::random_device{}()};
std::uniform_real_distribution<float> Element::norm_dist(0.0f, 1.0f);

// --- Construction ---
Element::Element(ElementType type, int x, int y)
	: color(ElementColors::getColorByElementType(type, x, y)),
	backup_color(color),
	type(type)
{}

// --- Getters/Setters ---
ElementType Element::getType() const { return type; }

bool Element::getStep() const { return step; }
void Element::setStep(bool value) { step = value; }

SDL_Color Element::getColor() const { return color; }
void Element::setColor(const SDL_Color& newColor) { color = newColor; }

float Element::getDensity() const { return density; };

// --- Random Direction Utility ---
int Element::getRandomDirection() {
	static std::uniform_int_distribution<int> dist(0, 1);
	return dist(rng) == 0 ? -1 : 1;
}

// --- Update Helper ---
bool Element::checkIfUpdated() {
	if (step == CellularMatrix::step) {
		return true;
	}
	step = CellularMatrix::step;
	return false;
}

void Element::swapElement(CellularMatrix& matrix, int x1, int y1, int x2, int y2) {
	std::swap(matrix.getElement(x1, y1), matrix.getElement(x2, y2));
	matrix.getElement(x2, y2)->setStep(CellularMatrix::step);
};

// --- Destroy Element Utility ---
void Element::destroyElement(CellularMatrix& matrix, int x, int y) {
	if (matrix.getElement(x, y)->getType() != EMPTY) {
		Element* emptyElement = createElementFromType(EMPTY, x, y);
		Element* oldElement = matrix.matrix[y][x];
		matrix.matrix[y][x] = emptyElement;
		delete oldElement;
	}
}

// --- Default update (should not be called directly) ---
void Element::update(CellularMatrix&, int, int) {
	return;
}