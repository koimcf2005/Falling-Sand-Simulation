// src/CellularMatrix.hpp
#ifndef CELLULAR_MATRIX_HPP
#define CELLULAR_MATRIX_HPP

#include "./elements/Element.hpp"
#include "./elements/ElementType.hpp"
#include "./elements/ElementColors.hpp"
#include "./elements/IMatrixAccess.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <thread>

//-------------------------------------------
// Cellular Matrix Class
//-------------------------------------------
class CellularMatrix : public IMatrixAccess {
public:
    //-------------------------------------------
    // Construction/Destruction
    //-------------------------------------------
    CellularMatrix(int width, int height);
    ~CellularMatrix();

    //-------------------------------------------
    // Grid Properties
    //-------------------------------------------
    std::vector<std::vector<Element*>> matrix;

    //-------------------------------------------
    // Element Management
    //-------------------------------------------
    void placeElement(int x, int y, ElementType type);
    void placeElementsInArea(int startX, int startY, int radius, ElementType type);

    // IMatrixAccess interface implementation
    bool isInBounds(int x, int y) const override;
    bool isEmpty(int x, int y) const;
    Element*& getElement(int x, int y) override;
    const Element* getElement(int x, int y) const override;
    void destroyElement(int x, int y) override;
    void swapElements(int x1, int y1, int x2, int y2) override;

    //-------------------------------------------
    // Simulation Update
    //-------------------------------------------
    void update();

    static void updateColumnRange(int startCol, int step, int colWidth, 
                                std::vector<std::vector<Element*>>& matrix, 
                                int width, int height);

    //-------------------------------------------
    // Rendering
    //-------------------------------------------
    void initializeRenderer(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);

private:
    const int WIDTH, HEIGHT;
    std::mt19937 rng{std::random_device{}()};

    //-------------------------------------------
    // Rendering Properties
    //-------------------------------------------
    SDL_Texture* renderTexture = nullptr;
    std::vector<Uint32> pixels;
};

#endif // CELLULAR_MATRIX_HPP