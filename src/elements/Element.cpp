// src/elements/Element.cpp
#include "src/elements/Element.hpp"

bool Element::s_Step = false;

// ========= Constructor =========
Element::Element(ElementType type, int x, int y)
	: m_Type(type),
	m_Color(ElementFactory::getColorByElementType(type, x, y)),
	m_OriginalColor(m_Color),
	m_Step(!s_Step),
	m_PosX(x),
	m_PosY(y)
{}

// ========= Element Metadata =========
ElementType Element::getType() const { return m_Type; }
std::string Element::getTypeString() const { return ElementFactory::getElementName(m_Type); }
SDL_Color Element::getColor() const { return m_Color; }
void Element::setColor(const SDL_Color& newColor) { m_Color = newColor; }

// ========= Simulation Step Tracking =========
bool Element::getHasUpdated() const { return m_Step == Element::s_Step; }
void Element::setAsUpdated() { m_Step = Element::s_Step; }

// ========= Positional Properties =========
int Element::getPosX() const { return m_PosX; }
int Element::getPosY() const { return m_PosY; }
void Element::setPosition(int x, int y) { m_PosX = x; m_PosY = y; }

// ========= Update State =========
bool Element::checkIfUpdated() {
	if (m_Step == s_Step) return true;
	m_Step = s_Step;
	return false;
}

// ========= Element Management =========
void Element::destroyElement(IMatrix& matrix, int x, int y) {
	matrix.destroyElement(x, y);
}

void Element::destroySelf(IMatrix& matrix) {
	matrix.destroyElement(m_PosX, m_PosY);
}