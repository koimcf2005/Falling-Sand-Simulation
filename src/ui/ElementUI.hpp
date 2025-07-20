// src/ui/ElementUI.hpp
#ifndef ELEMENT_UI_HPP
#define ELEMENT_UI_HPP

#include "src/elements/ElementFactory.hpp"
#include "src/core/Globals.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

/**
 * @brief Represents a button for selecting an element in the UI.
 */
struct ElementButton {
	ElementType type;      ///< The element type this button represents
	SDL_Rect rect;         ///< Button rectangle (position and size)
	std::string name;      ///< Display name for the element
	SDL_Color color;       ///< Button color
	bool isHovered;        ///< True if the mouse is over this button
	bool isSelected;       ///< True if this button is currently selected
	
	ElementButton(ElementType t, const std::string& n, SDL_Color c) 
		: type(t), name(n), color(c), isHovered(false), isSelected(false) {
		rect = {0, 0, 0, 0};
	}
};

/**
 * @brief UI overlay for selecting elements and displaying tooltips.
 * 
 * Handles rendering of element selection buttons, tooltips, and user interaction.
 */
class ElementUI {
public:
	/**
	 * @brief Construct a new ElementUI object.
	 * @param renderer SDL_Renderer pointer for rendering
	 */
	ElementUI(SDL_Renderer* renderer);

	/**
	 * @brief Destructor. Cleans up font resources.
	 */
	~ElementUI();
	
	/**
	 * @brief Initialize the UI (load font, populate buttons).
	 * @return true on success, false on failure
	 */
	bool initialize();

	/**
	 * @brief Cleanup font resources.
	 */
	void cleanup();
	
	/**
	 * @brief Handle SDL events for UI interaction (mouse clicks, etc).
	 * @param event SDL_Event reference
	 */
	void handleEvent(const SDL_Event& event);

	/**
	 * @brief Update button hover states based on mouse position.
	 * @param mouseX Mouse X in window coordinates
	 * @param mouseY Mouse Y in window coordinates
	 */
	void update(int mouseX, int mouseY);

	/**
	 * @brief Render the UI (buttons and tooltips).
	 */
	void render();
	
	/**
	 * @brief Toggle UI visibility.
	 */
	void toggleVisibility() { m_Visible = !m_Visible; }

	/**
	 * @brief Check if the UI is currently visible.
	 */
	bool isVisible() const { return m_Visible; }

	/**
	 * @brief Get the currently selected element type.
	 */
	ElementType getSelectedElement() const { return m_SelectedElement; }
	
	/**
	 * @brief Add a new element button to the UI.
	 * @param type Element type
	 * @param name Display name
	 */
	void addElement(ElementType type, const std::string& name);

	/**
	 * @brief Check if the mouse is over the UI.
	 * @param x Mouse X in window coordinates
	 * @param y Mouse Y in window coordinates
	 * @return true if mouse is over UI, false otherwise
	 */
	bool isMouseOverUI(int x, int y) const;

	// UI layout constants
	static constexpr int UI_PADDING = 8;           ///< Padding around the UI
	static constexpr int BUTTON_SIZE = 32;         ///< Size of each button (square)
	static constexpr int BUTTON_SPACING = 8;       ///< Spacing between buttons
	static constexpr int BUTTON_BORDER_WIDTH = 4;  ///< Thickness of button border
	static constexpr int TOOLTIP_FONT_SIZE = 16;   ///< Font size for tooltips
	static constexpr int TOOLTIP_MARGIN = 4;       ///< Margin above tooltip
	static constexpr int TOOLTIP_PADDING = 4;      ///< Padding inside tooltip
	static constexpr int TOOLTIP_ALPHA = 200;      ///< Tooltip background alpha
	
private:
	SDL_Renderer* mp_Renderer;        ///< SDL renderer pointer
	TTF_Font* mp_Font {nullptr};       ///< Loaded font for tooltips

	std::vector<ElementButton> m_Buttons; ///< List of element buttons
	ElementType m_SelectedElement;        ///< Currently selected element
	bool m_Visible;                       ///< UI is visible by default
	
	/**
	 * @brief Update the positions of all buttons.
	 */
	void updateButtonPositions();

	/**
	 * @brief Render a single button.
	 * @param button Button to render
	 */
	void renderButton(const ElementButton& button);

	/**
	 * @brief Render the tooltip for a hovered button.
	 * @param button Button for which to render the tooltip
	 */
	void renderTooltip(const ElementButton& button);

	/**
	 * @brief Calculate the bounding rectangle of the UI.
	 * @return SDL_Rect representing the UI bounds
	 */
	SDL_Rect calculateUIBounds() const;
};

#endif // ELEMENT_UI_HPP