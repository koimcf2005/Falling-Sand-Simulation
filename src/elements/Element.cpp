// src/elements/Element.cpp
#include "src/elements/Element.hpp"

class PhysicsParticle;

// ========= Static Members =========
std::mt19937 Element::rng{std::random_device{}()};
std::uniform_real_distribution<float> Element::norm_dist(0.0f, 1.0f);
bool Element::globalStep = false;

// ========= Constructor =========
Element::Element(ElementType type, int x, int y)
	: type(type),
	color(ElementFactory::getColorByElementType(type, x, y)),
	backup_color(color)
{}

// ========= Metadata Access =========
ElementType Element::getType() const { return type; }
SDL_Color Element::getColor() const { return color; }
std::string Element::getTypeString() const { return ElementFactory::getElementName(type); }
void Element::setColor(const SDL_Color& newColor) { color = newColor; }
float Element::getDensity() const { return density; }
float Element::getFriction() const { return friction; }
float Element::getImpactAbsorption() const { return impactAbsorption; }
float Element::getInertialResistance() const { return inertialResistance; }

// ========= Temperature System =========
float Element::getTemperature() const { return temperature; }
void Element::setTemperature(float temp) { temperature = temp; }
void Element::addTemperature(float temp) { temperature += temp; }
float Element::getTemperatureThreshold() const { return temperatureThreshold; }

void Element::reactToHeat(IMatrix&, int, int) {
	// Default: no reaction to heat
}

void Element::reactToCooling(IMatrix&, int, int) {
	// Default: no reaction to heat
}

bool Element::handleHeat(IMatrix& matrix, int x, int y) {
	if (shouldReactToHeat && temperature >= temperatureThreshold) {
		reactToHeat(matrix, x, y);
		shouldReactToHeat = false;
		return true;
	}
	else if (!shouldReactToHeat && temperature < temperatureThreshold) {
		reactToCooling(matrix, x, y);
		shouldReactToHeat = true;
		return true;
	}
	return false;
}

float Element::calculateAbsorption(IMatrix& matrix, int x1, int y1, int x2, int y2) {
	float elem1 = matrix.isInBounds(x1, y1) ?
		matrix.getElement(x1, y1)->getImpactAbsorption() : 0.1f;
	float elem2 = matrix.isInBounds(x2, y2) ?
		matrix.getElement(x2, y2)->getImpactAbsorption() : 0.1f;
	return std::clamp((1 - elem1) * (1 - elem2), 0.0f, 1.0f);
}

float Element::calculateFriction(IMatrix& matrix, int x1, int y1, int x2, int y2) {
	Element* elem1 = matrix.isInBounds(x1, y1) ? matrix.getElement(x1, y1) : nullptr;
	Element* elem2 = matrix.isInBounds(x2, y2) ? matrix.getElement(x2, y2) : nullptr;

	if ((elem1 && elem1->getType() == EMPTY) || (elem2 && elem2->getType() == EMPTY)) return 1.0f;

	float friction1 = elem1 ? elem1->getFriction() : 0.1f;
	float friction2 = elem2 ? elem2->getFriction() : 0.1f;
	return std::clamp(1 - (friction1 + friction2), 0.0f, 1.0f);
}

// ========= Update State =========
bool Element::getHasUpdated() const { return step == Element::globalStep; }
void Element::setAsUpdated() { step = Element::globalStep; }
void Element::setAsMoving() {
	movedThisFrame = true;
}

// ========= Utility =========
int Element::getRandomDirection() {
	static std::uniform_int_distribution<int> dist(0, 1);
	return dist(rng) == 0 ? -1 : 1;
}

bool Element::checkIfUpdated() {
	if (step == globalStep) return true;
	step = globalStep;
	return false;
}

void Element::swapElements(IMatrix& matrix, int x1, int y1, int x2, int y2) {
	if (x1 == x2 && y1 == y2) return; // no-op
	Element* from = matrix.getElement(x1, y1);
	Element* to   = matrix.getElement(x2, y2);

	from->setAsUpdated();
	to->setAsUpdated();
	from->setAsMoving();

	matrix.swapElements(x1, y1, x2, y2);
}

void Element::destroyElement(IMatrix& matrix, int x, int y) {
	matrix.destroyElement(x, y);
}

// ========= Movement =========
void Element::handleFalling(IMatrix& matrix, int x, int y) {
	if (canReplaceElement(matrix, x, y + 1)) {
		isMoving = true;
		velocity_y = std::clamp(velocity_y + GRAVITY, 0.0f, 10.0f);
		accumulated_y += velocity_y;
		int move_y = static_cast<int>(accumulated_y);

		if (move_y != 0) {
			int last_valid_y = y;
			for (int i = 1; i <= std::fabs(move_y); ++i) {
				int check_y = y + (move_y > 0 ? i : -i);
				if (canReplaceElement(matrix, x, check_y)) {
					last_valid_y = check_y;
				} else {
					break;
				}
			}
			if (last_valid_y != y) {
				swapElements(matrix, x, y, x, last_valid_y);
				affectAdjacentNeighbors(matrix, x, y);
				accumulated_y -= (last_valid_y - y);
			}
		}
	} else {
		handleWhileGrounded(matrix, x, y);
		velocity_y = 1.0f;
		accumulated_y = 0.0f;
	}
}

void Element::handleBuoyancy(IMatrix&, int, int) {
	// Default: no buoyancy
}

void Element::handleWhileGrounded(IMatrix&, int, int) {
	// Default: do nothing while grounded
}

void Element::affectAdjacentNeighbors(IMatrix& matrix, int x, int y) {
	if (matrix.isInBounds(x - 1, y)) {
		if (Element* left = matrix.getElement(x - 1, y)) {
			left->applyAdjacentNeighborEffect();
			matrix.activateChunk(x - 1, y);
		}
	}
	if (matrix.isInBounds(x + 1, y)) {
		if (Element* right = matrix.getElement(x + 1, y)) {
			right->applyAdjacentNeighborEffect();
			matrix.activateChunk(x + 1, y);
		}
	}
}

void Element::applyAdjacentNeighborEffect() {
	// Default: no effect
}

bool Element::canReplaceElement(IMatrix&, int, int) const {
	return false; // Default: not replaceable
}