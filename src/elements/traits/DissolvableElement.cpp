#include "src/elements/traits/DissolvableElement.hpp"
#include "src/elements/traits/SolvantElement.hpp"
#include "src/elements/Element.hpp"

void DissolvableElement::handleDissolving(IMatrix& matrix, int x, int y) {
	for (auto& solvant : m_Solvents) {
		ElementType solvantType = solvant.first;
		float solvantChance = solvant.second;

		for (int dx = -1; dx < 1; ++dx) {
			for (int dy = -1; dy < 1; ++dy) {
				int tx = x + dx, ty = y + dy;

				if (dx == 0 && dy == 0) continue;
				if (!matrix.isInBounds(tx, ty)) continue;

				auto neighbor = matrix.getElement(tx, ty);
				auto self = matrix.getElement(x, y);

				if (neighbor->getType() != solvantType) continue;

				// auto solvantElement = neighbor->as<SolvantElement>();

				// if (!solvantElement) {
				// 	throw std::runtime_error(self->getTypeString() + " has a listed solvant that does not inherit from the SolvableElement class!\n");
				// }

				// if (solvantElement->getDissolvedElement() != EMPTY) continue;

				if (!ElementRNG::getRandomChance(solvantChance)) continue;

				// solvantElement->setDissolvedElement(self->getType());
				matrix.destroyElement(x, y);
				return;
			}
		}
	}
}