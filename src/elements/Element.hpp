// src/elements/Element.hpp
#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_map>
#include "ElementType.hpp"
#include "ElementColors.hpp"
#include "../IMatrix.hpp"

class Element {
public:
    // --- Construction/Destruction ---
    Element(ElementType type, int x, int y);
    virtual ~Element() = default;

    // --- Core Interface ---
    ElementType getType() const;
    virtual void update(IMatrix& matrix, int x, int y) = 0;

    // --- Step Flag ---
    bool getStep() const;
    void setStep(bool value);

    // --- Color ---
    SDL_Color getColor() const;
    void setColor(const SDL_Color& newColor);

    float getDensity() const;

    // --- Static Step Flag ---
    static bool globalStep;

protected:
    // --- Random Number Generation ---
    static std::mt19937 rng;
    static std::uniform_real_distribution<float> norm_dist;
    static int getRandomDirection();

    // --- Update Helper ---
    bool checkIfUpdated();
    bool step = false;

    // --- Element Management ---
    static void swapElement(IMatrix& matrix, int x1, int y1, int x2, int y2);
    static void destroyElement(IMatrix& matrix, int x, int y);

    // --- Neighbor/Type Utilities ---
    static int isInsideElement(IMatrix& matrix, ElementType type, int x, int y);
    
    template<typename T>
    static bool isInstanceOf(Element* element) {
        return dynamic_cast<T*>(element) != nullptr;
    }

    // --- Element Properties ---
    SDL_Color color = {255, 255, 255, 255};
    SDL_Color backup_color = {255, 255, 255, 255};
    ElementType type = EMPTY;
    float density = 0.5f;
    bool wasMoving = true;
    bool isMoving = true;

    // --- Dissolution ---
    std::unordered_map<ElementType, float> dissolveChances;
    int dissolveThreshold = 1;

    // Returns a vector of pointers to neighboring elements of the specified type
    template<typename T>
    static std::vector<T*> getNeighborElementsOfType(IMatrix& matrix, ElementType type, int x, int y) {
        std::vector<T*> neighbors;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (matrix.isInBounds(nx, ny)) {
                    Element* elem = matrix.getElement(nx, ny);
                    if (elem && elem->getType() == type) {
                        T* casted = dynamic_cast<T*>(elem);
                        if (casted) {
                            neighbors.push_back(casted);
                        }
                    }
                }
            }
        }
        return neighbors;
    }
};

#endif // ELEMENT_HPP