#ifndef ELEMENT_FACTORY_HPP
#define ELEMENT_FACTORY_HPP

#include "src/elements/ElementTypes.hpp"
#include "src/components/Components.hpp"
#include "src/components/ComponentManager.hpp"

#include <array>
#include <random>
#include <map>

class Matrix;

/**
 * @brief A factory class responsible for registering and creating game elements.
 *
 * The ElementFactory provides static methods to register element types,
 * retrieve their properties, and create entities based on those types.
 * All element type data (e.g. color, name, textures) is stored internally.
 */
class ElementFactory {
public:
  /**
   * @brief Initializes the element registry and loads element textures.
   *
   * This should be called once at startup before creating elements.
   */
  static void initialize();

  static void initializeElementByType(ElementType type, entt::entity entity);

  static void updateElementByType(ElementType type, Matrix& matrix, entt::entity entity);

  /**
   * @brief Creates a new entity of the specified element type at the given position.
   *
   * The entity will have the necessary components (Element, Position, Velocity)
   * pre-attached.
   *
   * @param type The element type to create.
   * @param x The X coordinate.
   * @param y The Y coordinate.
   * @return The newly created EnTT entity.
   */
  static entt::entity createElementByType(const ElementType type, const int x, const int y);

  /**
   * @brief Returns the color of the given element at position (x, y).
   *
   * If the element has a texture, it samples the texture; otherwise,
   * it returns a randomized base color using an offset.
   *
   * @param type The element type.
   * @param x X coordinate in the world.
   * @param y Y coordinate in the world.
   * @return SDL_Color color value.
   */
  static SDL_Color getElementColor(const ElementType type, const int x, const int y);

  /**
   * @brief Returns the display name for the given element type.
   *
   * @param type The element type.
   * @return Name string, or "Unknown" if not found.
   */
  static std::string getElementName(const ElementType type);

  /**
   * @brief Cleans up any resources used by the ElementFactory.
   *
   * This should be called at shutdown to free textures and other resources.
   */
  static void cleanup();

private:
  /**
   * @brief Struct holding metadata for each element type.
   */
  struct ElementInfo {
    std::string name;           ///< Display name of the element.
    SDL_Color color;            ///< Base color.
    int colorOffset;            ///< Color variance offset.
    std::string texturePath;    ///< Path to optional texture file.

    ElementInfo()
		: name(""),
			color{0, 0, 0, 0},
			colorOffset(0),
			texturePath("")
		{}

    ElementInfo(const std::string& name, const SDL_Color& color,
                const int offset, const std::string& texture = "")
    : name(name),
			color(color),
			colorOffset(offset),
			texturePath(texture)
		{}
  };

  // Internal helper functions

  /**
   * @brief Samples the pixel color from a texture at (x, y).
   */
  static SDL_Color getTextureColor(const SDL_Surface* surface, const int x, const int y);

  /**
   * @brief Computes a randomized color based on the element's base color and offset.
   */
  static SDL_Color getOffsetColor(const ElementType type);

  /**
   * @brief Returns a random offset within ±offset using internal RNG.
   */
  static int getRandomOffset(const int offset);

  /**
   * @brief Registers a new element type and its metadata.
   *
   * @tparam T The element logic/class type.
   * @param type The enum value of the element.
   * @param name Display name.
   * @param color Base color.
   * @param colorOffset Maximum random color offset.
   * @param texturePath Optional texture file path.
   */
  static void registerElement(const ElementType type, const std::string& name,
                              const SDL_Color& color, const int colorOffset,
                              const std::string& texturePath = "");

  // Internal storage and maps

	///< Registered element metadata
  static std::array<ElementInfo, ElementType_Count> m_element_registry;
	///< Texture surface map
  static std::array<SDL_Surface*, ElementType_Count> m_texture_registry;
	///< Random number generator
  static std::mt19937 m_rng;
};

#endif // ELEMENT_FACTORY_HPP