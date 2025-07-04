// src/ui/ElementUI.hpp
#ifndef ELEMENT_UI_HPP
#define ELEMENT_UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "../elements/ElementType.hpp"
#include "../elements/ElementColors.hpp"

struct ElementButton {
    ElementType type;
    SDL_Rect rect;
    std::string name;
    SDL_Color color;
    bool isHovered;
    bool isSelected;
    
    ElementButton(ElementType t, const std::string& n, SDL_Color c) 
        : type(t), name(n), color(c), isHovered(false), isSelected(false) {
        rect = {0, 0, 0, 0};
    }
};

class ElementUI {
public:
    ElementUI();
    ~ElementUI();
    
    bool initialize(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    void cleanup();
    
    void handleEvent(const SDL_Event& event);
    void update(int mouseX, int mouseY);
    void render(SDL_Renderer* renderer);
    
    void toggleVisibility() { visible = !visible; }
    bool isVisible() const { return visible; }
    ElementType getSelectedElement() const { return selectedElement; }
    
    void addElement(ElementType type, const std::string& name);
    
private:
    static const int BUTTON_SIZE = 24;
    static const int BUTTON_SPACING = 4;
    static const int UI_PADDING = 8;
    
    std::vector<ElementButton> buttons;
    ElementType selectedElement;
    bool visible;
    
    TTF_Font* font;
    SDL_Renderer* renderer;
    int windowWidth, windowHeight;
    
    void updateButtonPositions();
    void renderButton(SDL_Renderer* renderer, const ElementButton& button);
    void renderTooltip(SDL_Renderer* renderer, const ElementButton& button);
    SDL_Rect calculateUIBounds() const;
};

#endif // ELEMENT_UI_HPP