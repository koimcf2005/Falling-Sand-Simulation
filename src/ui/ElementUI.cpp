// src/ui/ElementUI.cpp
#include "src/ui/ElementUI.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// Constructor/Destructor and Initialization
//------------------------------------------------------------------------------

ElementUI::ElementUI(SDL_Renderer* renderer) 
	: mp_Renderer(renderer),
	mp_Font(nullptr),
	m_SelectedElement(SAND),
	m_Visible(true)
{}

ElementUI::~ElementUI() {
	cleanup();
}

bool ElementUI::initialize() {
	// Load font for tooltips and button labels
	mp_Font = TTF_OpenFont("assets/fonts/PublicPixel.ttf", TOOLTIP_FONT_SIZE);
	if (!mp_Font) {
		std::cerr << "Failed to load mp_Font: " << TTF_GetError() << std::endl;
		return false;
	}
	
	// Add all registered elements as buttons (except EMPTY)
	std::vector<ElementType> registeredElements = ElementFactory::getRegisteredElements();
	for (ElementType type : registeredElements) {
		if (type != EMPTY) {
			std::string name = ElementFactory::getElementName(type);
			SDL_Color color = ElementFactory::getColorByElementType(type, 0, 0);
			m_Buttons.emplace_back(type, name, color);
		}
	}
		
	// Set first element as selected
	if (!m_Buttons.empty()) {
		m_Buttons[0].isSelected = true;
		m_SelectedElement = m_Buttons[0].type;
	}
	
	updateButtonPositions();
	return true;
}

void ElementUI::cleanup() {
	// Free font resource
	if (mp_Font) {
		TTF_CloseFont(mp_Font);
		mp_Font = nullptr;
	}
}

//------------------------------------------------------------------------------
// Button Management and Layout
//------------------------------------------------------------------------------

void ElementUI::addElement(ElementType type, const std::string& name) {
	SDL_Color color = ElementFactory::getColorByElementType(type, 0, 0);
	m_Buttons.emplace_back(type, name, color);
	updateButtonPositions();
}

void ElementUI::updateButtonPositions() {
	// Arrange buttons horizontally, centered at the top of the window
	if (m_Buttons.empty()) return;
	int totalWidth = m_Buttons.size() * BUTTON_SIZE + (m_Buttons.size() - 1) * BUTTON_SPACING;
	int startX = (Window::WIDTH - totalWidth) / 2;
	int startY = UI_PADDING;
	for (size_t i = 0; i < m_Buttons.size(); ++i) {
		m_Buttons[i].rect.x = startX + i * (BUTTON_SIZE + BUTTON_SPACING);
		m_Buttons[i].rect.y = startY;
		m_Buttons[i].rect.w = BUTTON_SIZE;
		m_Buttons[i].rect.h = BUTTON_SIZE;
	}
}

//------------------------------------------------------------------------------
// Event Handling and State Update
//------------------------------------------------------------------------------

void ElementUI::handleEvent(const SDL_Event& event) {
	if (!m_Visible) return;
	// Handle mouse click for button selection
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		int mouseX = event.button.x;
		int mouseY = event.button.y;
		for (auto& button : m_Buttons) {
			if (mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
				mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h) {
				for (auto& b : m_Buttons) b.isSelected = false;
				button.isSelected = true;
				m_SelectedElement = button.type;
				std::cout << "Selected " << button.name << std::endl;
				break;
			}
		}
	}
}

void ElementUI::update(int mouseX, int mouseY) {
	if (!m_Visible) return;
	// Update hover state for each button
	for (auto& button : m_Buttons) {
		button.isHovered = (mouseX >= button.rect.x && mouseX < button.rect.x + button.rect.w &&
						   mouseY >= button.rect.y && mouseY < button.rect.y + button.rect.h);
	}
}

//------------------------------------------------------------------------------
// Rendering
//------------------------------------------------------------------------------

void ElementUI::render() {
	if (!m_Visible) return;

	// Render semi-transparent background for the UI bar
	SDL_Rect uiBounds = calculateUIBounds();
	SDL_SetRenderDrawBlendMode(mp_Renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mp_Renderer, 0, 0, 0, 128);
	SDL_RenderFillRect(mp_Renderer, &uiBounds);

	// Render all element buttons
	for (const auto& button : m_Buttons) {
		renderButton(button);
	}

	// Render tooltip for hovered button (if any)
	for (const auto& button : m_Buttons) {
		if (button.isHovered) {
			renderTooltip(button);
			break;
		}
	}
}

void ElementUI::renderButton(const ElementButton& button) {
	// Draw button background
	SDL_SetRenderDrawColor(mp_Renderer, button.color.r, button.color.g, button.color.b, 255);
	SDL_RenderFillRect(mp_Renderer, &button.rect);

	// Lambda for drawing thick rectangle borders
	auto drawThickRect = [this](const SDL_Rect& rect, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		SDL_SetRenderDrawColor(mp_Renderer, r, g, b, a);
		for (int i = 0; i < thickness; ++i) {
			SDL_Rect rct = {rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
			if (rct.w > 0 && rct.h > 0)
				SDL_RenderDrawRect(mp_Renderer, &rct);
		}
	};

	// Draw border based on button state
	if (button.isSelected) {
		drawThickRect(button.rect, BUTTON_BORDER_WIDTH, 255, 255, 255, 255);
	} else if (button.isHovered) {
		drawThickRect(button.rect, BUTTON_BORDER_WIDTH, 200, 200, 200, 255);
	} else {
		drawThickRect(button.rect, BUTTON_BORDER_WIDTH, 100, 100, 100, 255);
	}
}

void ElementUI::renderTooltip(const ElementButton& button) {
	if (!mp_Font) return;

	// Render tooltip text for the hovered button
	SDL_Color textColor = {255, 255, 255, 255};
	SDL_Surface* textSurface = TTF_RenderText_Solid(mp_Font, button.name.c_str(), textColor);
	if (!textSurface) return;

	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mp_Renderer, textSurface);
	if (!textTexture) {
		SDL_FreeSurface(textSurface);
		return;
	}

	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	SDL_FreeSurface(textSurface);

	// Center tooltip horizontally with respect to the button, just below the button
	int tooltipX = button.rect.x + (button.rect.w - textWidth) / 2 - TOOLTIP_PADDING;
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
	SDL_SetRenderDrawBlendMode(mp_Renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mp_Renderer, 0, 0, 0, TOOLTIP_ALPHA);
	SDL_RenderFillRect(mp_Renderer, &tooltipRect);

	// Optionally render tooltip border (uncomment if desired)
	// SDL_SetRenderDrawColor(mp_Renderer, 255, 255, 255, 255);
	// SDL_RenderDrawRect(mp_Renderer, &tooltipRect);

	// Render tooltip text
	SDL_RenderCopy(mp_Renderer, textTexture, nullptr, &textRect);
	SDL_DestroyTexture(textTexture);
}

//------------------------------------------------------------------------------
// UI Bounds Calculation
//------------------------------------------------------------------------------

SDL_Rect ElementUI::calculateUIBounds() const {
	// Calculate the bounding rectangle of the UI bar
	if (m_Buttons.empty()) return {0, 0, 0, 0};
	int totalWidth = m_Buttons.size() * BUTTON_SIZE + (m_Buttons.size() - 1) * BUTTON_SPACING;
	return {
		(Window::WIDTH - totalWidth) / 2 - UI_PADDING,
		0,
		totalWidth + 2 * UI_PADDING,
		BUTTON_SIZE + 2 * UI_PADDING
	};
}

bool ElementUI::isMouseOverUI(int x, int y) const {
	// Check if the mouse is over the UI bar
	SDL_Rect uiBounds = calculateUIBounds();
	return (x >= uiBounds.x && x < uiBounds.x + uiBounds.w &&
			y >= uiBounds.y && y < uiBounds.y + uiBounds.h &&
			isVisible());
}