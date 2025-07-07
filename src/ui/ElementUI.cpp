// src/ui/ElementUI.cpp
#include "ElementUI.hpp"
#include <iostream>

ElementUI::ElementUI() 
	: selectedElement(SAND), visible(true), font(nullptr), renderer(nullptr),
	  windowWidth(0), windowHeight(0) {
}

ElementUI::~ElementUI() {
	cleanup();
}

bool ElementUI::initialize(SDL_Renderer* r, int winWidth, int winHeight) {
    renderer = r;
    windowWidth = winWidth;
    windowHeight = winHeight;
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Load font (smaller size for tooltip)
    font = TTF_OpenFont("assets/fonts/PublicPixel.ttf", TOOLTIP_FONT_SIZE);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Automatically add all registered elements from ElementFactory
    std::vector<ElementType> registeredElements = ElementFactory::getRegisteredElements();
    for (ElementType type : registeredElements) {
        // Skip EMPTY element in UI
        if (type != EMPTY) {
            std::string name = ElementFactory::getElementName(type);
            SDL_Color color = ElementFactory::getColorByElementType(type, 0, 0);
            buttons.emplace_back(type, name, color);
        }
    }
    
    // Set first element as selected
    if (!buttons.empty()) {
        buttons[0].isSelected = true;
        selectedElement = buttons[0].type;
    }
    
    updateButtonPositions();
    return true;
}

void ElementUI::cleanup() {
	if (font) {
		TTF_CloseFont(font);
		font = nullptr;
	}
	TTF_Quit();
}

void ElementUI::addElement(ElementType type, const std::string& name) {
	SDL_Color color = ElementFactory::getColorByElementType(type, 0, 0);
	buttons.emplace_back(type, name, color);
	updateButtonPositions();
}

void ElementUI::updateButtonPositions() {
	if (buttons.empty()) return;
	
	int totalWidth = buttons.size() * BUTTON_SIZE + (buttons.size() - 1) * BUTTON_SPACING;
	int startX = (windowWidth - totalWidth) / 2;
	int startY = UI_PADDING;
	
	for (size_t i = 0; i < buttons.size(); ++i) {
		buttons[i].rect.x = startX + i * (BUTTON_SIZE + BUTTON_SPACING);
		buttons[i].rect.y = startY;
		buttons[i].rect.w = BUTTON_SIZE;
		buttons[i].rect.h = BUTTON_SIZE;
	}
}

void ElementUI::handleEvent(const SDL_Event& event) {
	if (!visible) return;
	
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		int mouseX = event.button.x;
		int mouseY = event.button.y;
		
		for (auto& button : buttons) {
			if (mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
				mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h) {
				
				// Deselect all buttons
				for (auto& b : buttons) {
					b.isSelected = false;
				}
				
				// Select clicked button
				button.isSelected = true;
				selectedElement = button.type;
				std::cout << "Selected " << button.name << std::endl;
				break;
			}
		}
	}
}

void ElementUI::update(int mouseX, int mouseY) {
	if (!visible) return;
	
	for (auto& button : buttons) {
		button.isHovered = (mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
						   mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h);
	}
}

void ElementUI::render(SDL_Renderer* renderer) {
	if (!visible) return;
	
	// Render semi-transparent background
	SDL_Rect uiBounds = calculateUIBounds();
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_RenderFillRect(renderer, &uiBounds);
	
	// Render buttons
	for (const auto& button : buttons) {
		renderButton(renderer, button);
	}
	
	// Render tooltip for hovered button
	for (const auto& button : buttons) {
		if (button.isHovered) {
			renderTooltip(renderer, button);
			break;
		}
	}
}

void ElementUI::renderButton(SDL_Renderer* renderer, const ElementButton& button) {
	// Button background
	SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, 255);
	SDL_RenderFillRect(renderer, &button.rect);

	// Button border
	if (button.isSelected) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &button.rect);
		// Removed innerRect for single-thickness border
	} else if (button.isHovered) {
		SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
		SDL_RenderDrawRect(renderer, &button.rect);
	} else {
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderDrawRect(renderer, &button.rect);
	}
}

void ElementUI::renderTooltip(SDL_Renderer* renderer, const ElementButton& button) {
	if (!font) return;
	
	SDL_Color textColor = {255, 255, 255, 255};
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.name.c_str(), textColor);
	if (!textSurface) return;
	
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if (!textTexture) {
		SDL_FreeSurface(textSurface);
		return;
	}
	
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	SDL_FreeSurface(textSurface);
	
	// Center tooltip horizontally in the UI, just below the button UI
	int tooltipX = (windowWidth - textWidth) / 2 - TOOLTIP_PADDING;
	int tooltipY = button.rect.y + button.rect.h + TOOLTIP_MARGIN;
	SDL_Rect tooltipRect = {
		tooltipX,
		tooltipY,
		textWidth + 2 * TOOLTIP_PADDING,
		textHeight + 2 * TOOLTIP_PADDING
	};
	SDL_Rect textRect = {
		tooltipRect.x + TOOLTIP_PADDING,
		tooltipRect.y + TOOLTIP_PADDING,
		textWidth,
		textHeight
	};
	
	// Render tooltip background
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, TOOLTIP_ALPHA);
	SDL_RenderFillRect(renderer, &tooltipRect);
	
	// Render tooltip border
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &tooltipRect);
	
	// Render text
	SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
	SDL_DestroyTexture(textTexture);
}

SDL_Rect ElementUI::calculateUIBounds() const {
	if (buttons.empty()) return {0, 0, 0, 0};
	
	int totalWidth = buttons.size() * BUTTON_SIZE + (buttons.size() - 1) * BUTTON_SPACING;
	return {
		(windowWidth - totalWidth) / 2 - UI_PADDING,
		0,
		totalWidth + 2 * UI_PADDING,
		BUTTON_SIZE + 2 * UI_PADDING
	};
}

bool ElementUI::isMouseOverUI(int x, int y) const {
	SDL_Rect uiBounds = calculateUIBounds();
	return (x >= uiBounds.x && x < uiBounds.x + uiBounds.w &&
			y >= uiBounds.y && y < uiBounds.y + uiBounds.h &&
			isVisible());
}