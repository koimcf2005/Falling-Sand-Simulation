// src/elements/Element.cpp
#include "Element.hpp"
#include "ElementType.hpp"

// --- Static members ---
std::mt19937 Element::rng{std::random_device{}()};
std::uniform_real_distribution<float> Element::norm_dist(0.0f, 1.0f);
bool Element::globalStep = false;

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

float Element::getDensity() const { return density; }

// --- Random Direction Utility ---
int Element::getRandomDirection() {
    static std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng) == 0 ? -1 : 1;
}

// --- Update Helper ---
bool Element::checkIfUpdated() {
    if (step == globalStep) {
        return true;
    }
    step = globalStep;
    return false;
}

void Element::swapElement(IMatrixAccess& matrix, int x1, int y1, int x2, int y2) {
    matrix.swapElements(x1, y1, x2, y2);
    matrix.getElement(x2, y2)->setStep(globalStep);
}

// --- Destroy Element Utility ---
void Element::destroyElement(IMatrixAccess& matrix, int x, int y) {
    matrix.destroyElement(x, y);
}

// --- Default update ---
void Element::update(IMatrixAccess&, int, int) {
    return;
}