// src/elements/Element.cpp
#include "Element.hpp"
#include "ElementType.hpp"

// ==================== Static Members ====================
std::mt19937 Element::rng{std::random_device{}()};
std::uniform_real_distribution<float> Element::norm_dist(0.0f, 1.0f);
bool Element::globalStep = false;

// ==================== Constructor ====================
Element::Element(ElementType type, int x, int y)
	: color(ElementColors::getColorByElementType(type, x, y)),
	backup_color(color),
	type(type)
{}

// ==================== Getters / Setters ====================
ElementType Element::getType() const { return type; }

SDL_Color Element::getColor() const { return color; }
void Element::setColor(const SDL_Color& newColor) { color = newColor; }

float Element::getDensity() const { return density; }

bool Element::getHasUpdated() const { return step == Element::globalStep; }

void Element::setAsUpdated() { step = Element::globalStep; }

// ==================== Utilities ====================
int Element::getRandomDirection() {
	static std::uniform_int_distribution<int> dist(0, 1);
	return dist(rng) == 0 ? -1 : 1;
}

bool Element::checkIfUpdated() {
	if (step == globalStep) {
		return true;
	}
	step = Element::globalStep;
	return false;
}

void Element::swapElement(IMatrix& matrix, int x1, int y1, int x2, int y2) {
	matrix.swapElements(x1, y1, x2, y2);
	matrix.getElement(x1, y1)->setAsUpdated();
	matrix.getElement(x2, y2)->setAsUpdated();
}

void Element::destroyElement(IMatrix& matrix, int x, int y) {
	matrix.destroyElement(x, y);
}

// ==================== Update ====================
void Element::update(IMatrix&, int, int) {
	return;
}