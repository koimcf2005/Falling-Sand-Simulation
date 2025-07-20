// src/elements/movable/MovableElement.cpp
#include "src/elements/movable/MovableElement.hpp"

void MovableElement::setVelocityX(float velocityX) {
	m_VelocityX = std::clamp(velocityX, -32.0f, 32.0f);
}

void MovableElement::setVelocityY(float velocityY) {
	m_VelocityY = std::clamp(velocityY, -32.0f, 32.0f);
}

void MovableElement::addVelocityX(float velocityX) {
	m_VelocityX = std::clamp(m_VelocityX + velocityX, -32.0f, 32.0f);
}

void MovableElement::addVelocityY(float velocityY) {
	m_VelocityY = std::clamp(m_VelocityY + velocityY, -32.0f, 32.0f);
}

float MovableElement::getVelocityX() const { return m_VelocityX; }
float MovableElement::getVelocityY() const { return m_VelocityY; }

bool MovableElement::getIsMoving() const { return m_IsMoving; }
void MovableElement::setIsMoving(bool isMoving) { m_IsMoving = isMoving; }

float MovableElement::getDensity() const { return m_Density; }

void MovableElement::swapWithElement(IMatrix& matrix, int x, int y) {
	if (x == m_PosX && y == m_PosY) return;
	Element* target = matrix.getElement(x, y);

	setAsUpdated();
	setIsMoving(true);
	setMovedThisFrame(true);

	target->setAsUpdated();
	if (auto movable = target->as<MovableElement>()) {
		movable->setIsMoving(true);
		movable->setMovedThisFrame(true);
	}

	matrix.swapElements(m_PosX, m_PosY, x, y);
}

void MovableElement::swapElements(IMatrix& matrix, int x1, int y1, int x2, int y2) {
	if (x1 == x2 && y1 == y2) return;
	Element* elem1 = matrix.getElement(x1, y1);
	Element* elem2 = matrix.getElement(x2, y2);

	if (auto movable = elem1->as<MovableElement>()) {
		movable->setIsMoving(true);
	}
	
	if (auto movable = elem2->as<MovableElement>()) {
		movable->setIsMoving(true);
	}

	matrix.swapElements(x1, y1, x2, y2);
}

void MovableElement::affectAdjacentNeighbors(IMatrix& matrix) {
	if (matrix.isInBounds(m_PosX - 1, m_PosY)) {
		if (Element* left = matrix.getElement(m_PosX - 1, m_PosY)) {
			if (auto movable = left->as<MovableElement>()) {
				movable->recieveNeighborEffect();
			}
			matrix.activateChunk(m_PosX - 1, m_PosY);
		}
	}
	if (matrix.isInBounds(m_PosX + 1, m_PosY)) {
		if (Element* right = matrix.getElement(m_PosX + 1, m_PosY)) {
			if (auto movable = right->as<MovableElement>()) {
				movable->recieveNeighborEffect();
			}
			matrix.activateChunk(m_PosX + 1, m_PosY);
		}
	}
}