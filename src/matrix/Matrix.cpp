#include "src/matrix/Matrix.hpp"

#include "src/elements/ElementFactory.hpp"
#include "src/renderer/Renderer.hpp"
#include "src/core/Utilities.hpp"

#include <iostream>
#include <vector>
#include <set>

//-------------------------------------------
// Constructor and Destructor
//-------------------------------------------

/**
 * @brief Constructs the Matrix object, initializing chunks and elements.
 */
Matrix::Matrix()
: m_matrix{},
  m_chunks{},
  m_debug_mode{false}
{
  for (int chunk_y = 0; chunk_y < Chunks::CHUNKS_Y; ++chunk_y) {
    for (int chunk_x = 0; chunk_x < Chunks::CHUNKS_X; ++chunk_x) {
      m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X] = Chunk(chunk_x, chunk_y);
    }
  }

  int element_count = 0;
  for (int y = 0; y < Simulation::HEIGHT; ++y) {
    for (int x = 0; x < Simulation::WIDTH; ++x) {
      m_matrix[x + y * Simulation::WIDTH] = ElementFactory::createElementByType(EMPTY, x, y);
      element_count++;
    }
  }
  std::cout << element_count << " Elements initialized." << std::endl;
}

//-------------------------------------------
// Main Update Loop
//-------------------------------------------

/**
 * @brief Updates the simulation by processing elements and committing chunk updates.
 */
void Matrix::update() {
  auto process = [&](const int x, const int y) {
    entt::entity entity = getEntity(x, y);
    Element& element = ComponentManager::getComponent<Element>(entity);
    if (element.step != s_matrix_step) {
      ElementFactory::updateElementByType(element.type, *this, entity, x, y);
      element.step = s_matrix_step;
    }
  };

  for (int y = Simulation::HEIGHT - 1; y >= 0; --y) {
    if (s_matrix_step) {
      for (int x = 0; x < Simulation::WIDTH; ++x) process(x, y);
    }
    else {
      for (int x = Simulation::WIDTH - 1; x >= 0; --x) process(x, y);
    }
  }
  for (auto& chunk : m_chunks) chunk.commitUpdateRect();
  s_matrix_step = !s_matrix_step;
}

int Matrix::s_debug_index = -1;
void Matrix::updateCellByCell(const int step_count) {
  std::cout << s_debug_index << '\n';
  for (int step = 0; step < step_count; ++step) {
    int x = s_debug_index % Simulation::WIDTH;
    int y = s_debug_index / Simulation::WIDTH;

    entt::entity entity = m_matrix[s_debug_index];
    Element& element = ComponentManager::getComponent<Element>(entity);
    if (element.step != s_matrix_step) {
      ElementFactory::updateElementByType(element.type, *this, entity, x, y);
      element.step = s_matrix_step;
    }
    --s_debug_index;
    if (s_debug_index < 0) {
      s_debug_index = Simulation::WIDTH * Simulation::HEIGHT - 1;
      s_matrix_step = !s_matrix_step;
    }
  }
}

//-------------------------------------------
// Element Management
//-------------------------------------------

/**
 * @brief Checks if the given coordinates are within the simulation bounds.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return True if the coordinates are within bounds, false otherwise.
 */
bool Matrix::isInBounds(const int x, const int y) const {
  return x >= 0 && x < Simulation::WIDTH && y >= 0 && y < Simulation::HEIGHT;
}

/**
 * @brief Checks if the given coordinates are empty (contain no element).
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return True if the coordinates are empty, false otherwise.
 */
bool Matrix::isEmpty(const int x, const int y) const {
  return getElement(x, y).type == EMPTY;
}

/**
 * @brief Places an element of the specified type at the given coordinates.
 * @param type The type of element to place.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 */
void Matrix::placeElement(const ElementType type, const int x, const int y) {
  if (!isInBounds(x, y)) return;
  entt::entity entity = getEntity(x, y);
  Element& element = ComponentManager::getComponent<Element>(entity);
  if (element.type == type) return;

  ComponentManager::destroyEntity(entity);
  m_matrix[x + y * Simulation::WIDTH] = ElementFactory::createElementByType(type, x, y);
  updateChunk(x, y);
}

/**
 * @brief Places elements of the specified type in a circular area.
 * @param type The type of element to place.
 * @param x Center X-coordinate.
 * @param y Center Y-coordinate.
 * @param radius Radius of the circular area.
 */
void Matrix::placeElementsInArea(const ElementType type, const int x, const int y, const int radius) {
  int r2 = std::max(1, radius * radius - 1);
  if (r2 == 1) {
    placeElement(type, x, y);
    return;
  }
  for (int ty = y - radius; ty <= y + radius; ++ty) {
    for (int tx = x - radius; tx <= x + radius; ++tx) {
      int dx = tx - x;
      int dy = ty - y;
      if (dx * dx + dy * dy <= r2) {
        placeElement(type, tx, ty);
      }
    }
  }
}

/**
 * @brief Swaps the entities at two given coordinates.
 * @param x1 X-coordinate of the first entity.
 * @param y1 Y-coordinate of the first entity.
 * @param x2 X-coordinate of the second entity.
 * @param y2 Y-coordinate of the second entity.
 */
void Matrix::swapEntities(const int x1, const int y1, const int x2, const int y2) {
  if (!isInBounds(x1, y1) || !isInBounds(x2, y2)) {
    return;
  }

  if (x1 == x2 && y1 == y2) {
    return;
  }

  entt::entity entity1 = m_matrix[x1 + y1 * Simulation::WIDTH];
  entt::entity entity2 = m_matrix[x2 + y2 * Simulation::WIDTH];
  Element& element1 = ComponentManager::getComponent<Element>(entity1);
  Element& element2 = ComponentManager::getComponent<Element>(entity2);

  m_matrix[x1 + y1 * Simulation::WIDTH] = m_matrix[x2 + y2 * Simulation::WIDTH];
  m_matrix[x2 + y2 * Simulation::WIDTH] = entity1;

  element1.step = s_matrix_step;
  element2.step = s_matrix_step;

  updateChunk(x1, y1);
  updateChunk(x2, y2);

  if (auto* movementState = ComponentManager::getComponentIfExists<MovementState>(entity1)) {
    movementState->setMovedThisFrame(true);
    movementState->setMoving(true);
  }

  if (auto* movementState = ComponentManager::getComponentIfExists<MovementState>(entity2)) {
    movementState->setMovedThisFrame(true);
    movementState->setMoving(true);
  }
}

/**
 * @brief Updates the chunk containing the given coordinates.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 */
void Matrix::updateChunk(const int x, const int y) {
  Chunk& chunk = m_chunks[getChunkX(x) + getChunkY(y) * Chunks::CHUNKS_X];
  chunk.addNewPosition(x, y);
}

//-------------------------------------------
// Rendering and Debug
//-------------------------------------------

/**
 * @brief Toggles the debug mode for rendering.
 */
void Matrix::toggleDebugMode() {
  if (m_debug_mode) {
    m_debug_mode = false;
    s_debug_index = -1;
  }
  else {
    m_debug_mode = true;
    s_debug_index = Simulation::WIDTH * Simulation::HEIGHT - 1;
  }
}

//-------------------------------------------
// Global Static Step
//-------------------------------------------

/**
 * @brief Static member variable to track the simulation step state.
 */
bool Matrix::s_matrix_step = false;

/**
 * @brief Gets the current simulation step state.
 * @return True if the current step is active, false otherwise.
 */
bool Matrix::getStep() {
  return s_matrix_step;
}

int Matrix::getDebugIndex() {
  return s_debug_index;
}

//-------------------------------------------
// Chunk Getters
//-------------------------------------------

/**
 * @brief Gets the chunk X-coordinate for a given simulation X-coordinate.
 * @param x Simulation X-coordinate.
 * @return Chunk X-coordinate.
 */
int Matrix::getChunkX(const int x) const { return x / Chunks::CHUNK_SIZE; }

/**
 * @brief Gets the chunk Y-coordinate for a given simulation Y-coordinate.
 * @param y Simulation Y-coordinate.
 * @return Chunk Y-coordinate.
 */
int Matrix::getChunkY(const int y) const { return y / Chunks::CHUNK_SIZE; }

/**
 * @brief Gets a reference to a chunk at the specified chunk coordinates.
 * @param chunk_x Chunk X-coordinate.
 * @param chunk_y Chunk Y-coordinate.
 * @return Reference to the Chunk object.
 */
Chunk& Matrix::getChunk(const int chunk_x, const int chunk_y) { return m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X]; }

/**
 * @brief Gets a constant reference to a chunk at the specified chunk coordinates.
 * @param chunk_x Chunk X-coordinate.
 * @param chunk_y Chunk Y-coordinate.
 * @return Constant reference to the Chunk object.
 */
const Chunk& Matrix::getChunk(const int chunk_x, const int chunk_y) const { return m_chunks[chunk_x + chunk_y * Chunks::CHUNKS_X]; }

const std::array<Chunk, Chunks::CHUNKS_X * Chunks::CHUNKS_Y>& Matrix::getChunks() const { return m_chunks; }

//-------------------------------------------
// Element Getters
//-------------------------------------------

/**
 * @brief Gets a reference to the element at the specified coordinates.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return Reference to the Element object.
 * @throws std::out_of_range if the coordinates are out of bounds.
 */
Element& Matrix::getElement(const int x, const int y) {
  if (!isInBounds(x, y)) {
    throw std::out_of_range("Matrix::getElement: coordinates out of bounds");
  }
  return ComponentManager::getComponent<Element>(m_matrix[x + y * Simulation::WIDTH]);
}

/**
 * @brief Gets a constant reference to the element at the specified coordinates.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return Constant reference to the Element object.
 * @throws std::out_of_range if the coordinates are out of bounds.
 */
Element& Matrix::getElement(const int x, const int y) const {
  if (!isInBounds(x, y)) {
    throw std::out_of_range("Matrix::getElement: coordinates out of bounds");
  }
  return ComponentManager::getComponent<Element>(m_matrix[x + y * Simulation::WIDTH]);
}

//-------------------------------------------
// Entity Getters
//-------------------------------------------

/**
 * @brief Gets the entity at the specified coordinates.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return The entity at the given coordinates, or entt::null if out of bounds.
 */
entt::entity Matrix::getEntity(const int x, const int y) {
  if (!isInBounds(x, y)) {
    return entt::null;
  }
  return m_matrix[x + y * Simulation::WIDTH];
}

/**
 * @brief Gets the entity at the specified coordinates (const version).
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return The entity at the given coordinates, or entt::null if out of bounds.
 */
entt::entity Matrix::getEntity(const int x, const int y) const {
  if (!isInBounds(x, y)) {
    return entt::null;
  }
  return m_matrix[x + y * Simulation::WIDTH];
}

//-------------------------------------------
// Chunk Helper Functions
//-------------------------------------------

/**
 * @brief Checks if the given chunk coordinates are valid.
 * @param chunk_x Chunk X-coordinate.
 * @param chunk_y Chunk Y-coordinate.
 * @return True if the chunk coordinates are valid, false otherwise.
 */
bool Matrix::isValidChunk(const int chunk_x, const int chunk_y) const {
  return chunk_x >= 0 && chunk_x < Chunks::CHUNKS_X && chunk_y >= 0 && chunk_y < Chunks::CHUNKS_Y;
}