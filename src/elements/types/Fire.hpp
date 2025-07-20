#ifndef FIRE_HPP
#define FIRE_HPP

#include "src/elements/static/StaticElement.hpp"
#include "src/particles/ParticleManager.hpp"

class Fire : public StaticElement {
public:
	struct FuelType {
		ElementType type;
		float chanceOfConsumption;
		int lifeFromConsumption;
		ElementType spawnOnDeath;
		float chanceToSpawnOnDeath;
		int chanceToSpawnSmoke;
	};
	const FuelType m_Fuels[2] = {
		{WOOD, 0.01f, 100, ASH, 0.1f, 1},
		{OIL, 0.8f, 0, EMPTY, 1.0f, -1}
	};

	float m_ChanceToSpawnParticle;
	float m_ChanceToSpawnSmoke;
	float m_LifeTime;

	Fire(int x, int y) : StaticElement(ElementType::FIRE, x, y) {
		// Explicitly initialize all member variables
		// (already done above, but do it here for safety)
		m_ChanceToSpawnParticle = 0.5f;
		m_ChanceToSpawnSmoke = 0.5f;
		m_LifeTime = 15;
	}

	void update(IMatrix& matrix) override {
		if (checkIfUpdated()) return;
		matrix.activateChunk(m_PosX, m_PosY);
		--m_LifeTime;
		if (m_LifeTime <= 0) {
			destroySelf(matrix);
			return;
		}

		// More natural fire color palette with weighted random
		int colorVariant = ElementRNG::getRandomInt(0, 9);
		switch (colorVariant) {
			case 1: // Pale yellow
				m_Color = {255, 240, 128, 215};
				break;
			case 2: // Yellow
				m_Color = {255, 220, 0, 215};
				break;
			case 3: // Orange-yellow
				m_Color = {255, 180, 40, 215};
				break;
			case 4: // Orange
				m_Color = {255, 140, 0, 215};
				break;
			case 5: // Orange-red
				m_Color = {255, 100, 0, 215};
				break;
		}

		if (!matrix.isInBounds(m_PosX, m_PosY - 1)) return;
		if (matrix.getElement(m_PosX, m_PosY - 1)->getType() == FIRE) return;

		if (ElementRNG::getRandomChance(m_ChanceToSpawnParticle)) {
			int w = ElementRNG::getRandomInt(1, 2);
			int h = ElementRNG::getRandomInt(1, 2);
			float dir = (ElementRNG::getRandomInt(0, 1) == 0) ? -1.0f : 1.0f;
			float vx = ElementRNG::getRandomFloat(0.0f, 0.3f) * dir;
			float vy = -ElementRNG::getRandomFloat(0.5f, 1.5f); // upward
			ParticleManager::spawnParticle({
				m_PosX, m_PosY,
				w, h,
				{m_Color.r, m_Color.g, m_Color.b, 215},
				vx, vy,
				0, 0,
				10,
				1.0f,
				0.4f
			});
		}

		if (ElementRNG::getRandomChance(m_ChanceToSpawnSmoke)) {
			matrix.placeElement(m_PosX, m_PosY - 1, SMOKE);
		}

	}
};

#endif // FIRE_HPP
