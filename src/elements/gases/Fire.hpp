// src/elements/gases/Fire.hpp
#ifndef FIRE_HPP
#define FIRE_HPP

#include "src/elements/Element.hpp"

class Fire : public Element {
public:
    Fire(int x, int y);
    void update(IMatrix& matrix, int x, int y) override;

private:
    void spreadHeat(IMatrix& matrix, int x, int y);
    void consumeFuel(IMatrix& matrix, int x, int y);
    void generateSmoke(IMatrix& matrix, int x, int y);
    bool hasFuelNearby(IMatrix& matrix, int x, int y);
    void updateFlameColor();
    
    int lifetime = 100 + (rng() % 50); // Random lifetime 100-150 frames
    int smokeGenerationTimer = 0;
    static std::uniform_int_distribution<int> colorDist;
};

#endif // FIRE_HPP