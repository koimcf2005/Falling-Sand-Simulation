// src/elements/gases/Fire.cpp
#include "Fire.hpp"
#include "src/elements/EmptyElement.hpp"
#include "src/elements/gases/Smoke.hpp"

std::uniform_int_distribution<int> Fire::colorDist(0, 2);

Fire::Fire(int x, int y) : Element(FIRE, x, y) {
	density = 0.1f; // Very light, rises up
	temperature = 800.0f; // Very hot
	temperatureThreshold = 1000.0f; // Won't react to its own heat
	updateFlameColor();
}

void Fire::addLife(int frames) {
	lifetime += frames;
}

void Fire::setFramesPerSmokeSpawn(int frames) {
	framesPerSmokeSpawn = frames;
}


void Fire::update(IMatrix& matrix, int x, int y) {
	if (checkIfUpdated()) return;
	matrix.activateChunk(x, y);

	// Check for fuel nearby
	if (!hasFuelNearby(matrix, x, y)) {
		lifetime -= 1; // Die without fuel
	}

	// Die if lifetime is over
	if (lifetime <= 0) {
		if (spawnOnDeath == EMPTY || norm_dist(rng) > chanceToSpawnOnDeath) {
			matrix.placeElement(x, y, EMPTY);
		}
		else {
			matrix.placeElement(x, y, spawnOnDeath);
		}
		return;
	}

	// Spread heat to nearby elements
	spreadHeat(matrix, x, y);
	
	// Generate smoke occasionally
	generateSmoke(matrix, x, y);
	
	// Update flame color for visual variety
	updateFlameColor();
	
	// Fire rises (buoyancy)
	handleBuoyancy(matrix, x, y);
}

void Fire::spreadHeat(IMatrix& matrix, int x, int y) {
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			
			int nx = x + dx;
			int ny = y + dy;
			
			if (matrix.isInBounds(nx, ny)) {
				Element* neighbor = matrix.getElement(nx, ny);
				if (neighbor && neighbor->getType() != EMPTY) {
					// Add heat based on distance
					float heatTransfer = 10.0f / (abs(dx) + abs(dy) + 1);
					neighbor->addTemperature(heatTransfer);
				}
			}
		}
	}
}

void Fire::generateSmoke(IMatrix& matrix, int x, int y) {
	if (framesPerSmokeSpawn == -1) return;

	smokeGenerationTimer++;
	if (smokeGenerationTimer >= framesPerSmokeSpawn) { // Generate smoke every 5 frames
		smokeGenerationTimer = 0;
		
		// Try to place smoke above the fire
		if (matrix.isInBounds(x, y - 1) && matrix.isEmpty(x, y - 1)) {
			matrix.placeElement(x, y - 1, SMOKE);
		}
	}
}

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
	FuelType(WOOD, 0.01f, 1000, COAL, 0.1f, 1),
	FuelType(OIL, 0.8f,    0, EMPTY, 1.0f, -1)
};

// Checks for nearby fuel elements and processes combustion behavior
bool Fire::hasFuelNearby(IMatrix& matrix, int x, int y) {
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			int nx = x + dx;
			int ny = y + dy;

			if (!matrix.isInBounds(nx, ny)) continue;

			Element* neighbor = matrix.getElement(nx, ny);
			if (!neighbor) continue;

			ElementType type = neighbor->getType();

			for (const auto& fuel : fuels) {
				if (fuel.type != type) continue;

				// Check if temperature is high enough to ignite
				if (neighbor->getTemperature() >= neighbor->getTemperatureThreshold() * 0.5f) {
					
					// Attempt combustion based on chance
					if (norm_dist(rng) < fuel.chanceOfConsumption) {
						matrix.placeElement(nx, ny, FIRE);

						Fire* newFire = dynamic_cast<Fire*>(matrix.getElement(nx, ny));
						if (newFire) {
							newFire->addLife(fuel.lifeFromConsumption);
							newFire->setFramesPerSmokeSpawn(fuel.framesPerSmokeSpawn);
							newFire->spawnOnDeath = fuel.spawnOnDeath;
							newFire->chanceToSpawnOnDeath = fuel.chanceToSpawnOnDeath;
						}
					}
				}

				// Found valid fuel nearby regardless of whether it ignites
				return true;
			}
		}
	}

	return false;
}

void Fire::updateFlameColor() {
	// Vary between yellow, orange, and red flames
	int colorVariant = colorDist(rng);
	switch (colorVariant) {
		case 0: // Yellow flame
			color = {255, 255, 0, 200};
			break;
		case 1: // Orange flame  
			color = {255, 165, 0, 200};
			break;
		case 2: // Red flame
			color = {255, 69, 0, 200};
			break;
	}
}