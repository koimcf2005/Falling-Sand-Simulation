// src/elements/gases/Fire.hpp
#ifndef FIRE_HPP
#define FIRE_HPP

#include "src/elements/Element.hpp"

class Fire : public Element {
public:
	Fire(int x, int y);
	void update(IMatrix& matrix, int x, int y) override;
	void addLife(int frames);
	void setFramesPerSmokeSpawn(int frames);

private:
	void spreadHeat(IMatrix& matrix, int x, int y);
	void generateSmoke(IMatrix& matrix, int x, int y);
	bool hasFuelNearby(IMatrix& matrix, int x, int y);
	void updateFlameColor();
	
	// Describes a flammable element and its behavior when consumed by fire.
	struct FuelType {
		ElementType type;                // Type of fuel element (e.g., WOOD, OIL)
		float chanceOfConsumption;       // Probability that the fuel is consumed
		int lifeFromConsumption;         // How much lifetime the fire gains
		ElementType spawnOnDeath;        // What element is spawned after burning
		float chanceToSpawnOnDeath;      // Probability to spawn the above element
		int framesPerSmokeSpawn;         // How often smoke is produced (or -1 for none)

		FuelType(ElementType t, float coc, int lfc, ElementType sod, float ctsod, int fpss)
			: type{t},
			chanceOfConsumption{coc},
			lifeFromConsumption{lfc},
			spawnOnDeath{sod},
			chanceToSpawnOnDeath{ctsod},
			framesPerSmokeSpawn{fpss}
		{}
	};

	// List of known flammable fuels and their combustion properties
	const std::vector<FuelType> fuels = {
		FuelType(WOOD, 0.01f, 1000, ASH, 0.1f, 1),
		FuelType(OIL, 0.8f,    0, EMPTY, 1.0f, -1)
	};

	ElementType spawnOnDeath = EMPTY;
	float chanceToSpawnOnDeath = 0;

	int lifetime = 10 + (rng() % 10); // Random lifetime 10-20 frames
	int smokeGenerationTimer = 0;
	int framesPerSmokeSpawn = 5;
	static std::uniform_int_distribution<int> colorDist;
};

#endif // FIRE_HPP