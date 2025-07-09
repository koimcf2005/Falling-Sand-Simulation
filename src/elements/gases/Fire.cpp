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

void Fire::update(IMatrix& matrix, int x, int y) {
    if (checkIfUpdated()) return;

    // Check for fuel nearby
    if (!hasFuelNearby(matrix, x, y)) {
        lifetime -= 2; // Die faster without fuel
    } else {
        lifetime--; // Normal decay
    }

    // Die if lifetime is over
    if (lifetime <= 0) {
        destroyElement(matrix, x, y);
        return;
    }

    // Spread heat to nearby elements
    spreadHeat(matrix, x, y);
    
    // Consume fuel
    consumeFuel(matrix, x, y);
    
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
                    float heatTransfer = 50.0f / (abs(dx) + abs(dy) + 1);
                    neighbor->addTemperature(heatTransfer);
                    
                    // Check if neighbor should react to heat
                    if (neighbor->getTemperature() > neighbor->getTemperatureThreshold()) {
                        neighbor->reactToHeat(matrix, nx, ny);
                    }
                }
            }
        }
    }
}

void Fire::consumeFuel(IMatrix& matrix, int x, int y) {
    // for (int dx = -1; dx <= 1; ++dx) {
    //     for (int dy = -1; dy <= 1; ++dy) {
    //         if (dx == 0 && dy == 0) continue;
            
    //         int nx = x + dx;
    //         int ny = y + dy;
            
    //         if (matrix.isInBounds(nx, ny)) {
    //             Element* neighbor = matrix.getElement(nx, ny);
    //             if (neighbor) {
    //                 ElementType type = neighbor->getType();
                    
    //                 // Consume fuel with some probability
    //                 if ((type == WOOD || type == OIL) && norm_dist(rng) < 0.05f) {
    //                     destroyElement(matrix, nx, ny);
    //                     lifetime += 20; // Extend life when consuming fuel
    //                 }
    //             }
    //         }
    //     }
    // }
}

void Fire::generateSmoke(IMatrix& matrix, int x, int y) {
    smokeGenerationTimer++;
    if (smokeGenerationTimer >= 5) { // Generate smoke every 5 frames
        smokeGenerationTimer = 0;
        
        // Try to place smoke above the fire
        if (matrix.isInBounds(x, y - 1) && matrix.isEmpty(x, y - 1)) {
            matrix.placeElement(x, y - 1, SMOKE);
        }
    }
}

bool Fire::hasFuelNearby(IMatrix& matrix, int x, int y) {
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dy = -2; dy <= 2; ++dy) {
            int nx = x + dx;
            int ny = y + dy;
            
            if (matrix.isInBounds(nx, ny)) {
                Element* neighbor = matrix.getElement(nx, ny);
                if (neighbor) {
                    ElementType type = neighbor->getType();
                    if (type == WOOD || type == OIL) {
                        return true;
                    }
                }
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