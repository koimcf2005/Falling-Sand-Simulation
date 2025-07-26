#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "src/core/Globals.hpp"
#include "src/matrix/Chunk.hpp"
#include "src/elements/ElementTypes.hpp"
#include "src/components/Components.hpp"
#include "src/components/ComponentManager.hpp"

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <random>
#include <array>

class Matrix {
public:
  //-------------------------------------------
  // Constructor and Destructor
  //-------------------------------------------
	Matrix();
	~Matrix();

  //-------------------------------------------
  // Main update loop
  //-------------------------------------------
	void update();

  //-------------------------------------------
  // Element Management 
  //-------------------------------------------
	bool isInBounds(const int x, const int y) const;

	bool isEmpty(const int x, const int y) const;

	void placeElement(const ElementType type, const int x, const int y);
	void placeElementsInArea(const ElementType type, const int x, const int y, const int radius);

	void swapEntities(const int x1, const int y1, const int x2, const int y2);

	void activateChunk(const int x, const int y);

	//-------------------------------------------
  // Rendering and Debug 
  //-------------------------------------------
	void toggleDebugMode();

	SDL_Texture* getTexture() const;
	void initializeTexture(SDL_Renderer* renderer);
	void updateTexture();
	
	//-------------------------------------------
  // Global Static Step
  //-------------------------------------------
	static bool getStep();

	//-------------------------------------------
  // Chunk Getters
  //-------------------------------------------
	int getChunkX(const int x) const;
	int getChunkY(const int y) const;

	Chunk& getChunk(const int chunk_x, const int chunk_y);
	const Chunk& getChunk(const int chunk_x, const int chunk_y) const;

	int getActiveChunkCount() const;

	//-------------------------------------------
  // Element Getters 
  //-------------------------------------------
	Element& getElement(const int x, const int y);
	Element& getElement(const int x, const int y) const;

	//-------------------------------------------
  // Entity Getters 
  //-------------------------------------------
	entt::entity getEntity(const int x, const int y);
	entt::entity getEntity(const int x, const int y) const;

	template<typename Component>
	bool entityHasComponent(const int x, const int y) {
		if (!isInBounds(x, y)) {
			return false;
		}
		return ComponentManager::hasComponent<Component>(getEntity(x, y));
	}

	template<typename Component>
	Component& getEntityComponent(const int x, const int y) {
		if (!isInBounds(x, y)) {
			throw std::out_of_range("Matrix::getEntityComponent: coordinates out of bounds");
		}
		return ComponentManager::getComponent<Component>(m_matrix[x + y * Simulation::WIDTH]);
	}

	template<typename Component>
	Component& getEntityComponent(const int x, const int y) const {
		if (!isInBounds(x, y)) {
			throw std::out_of_range("Matrix::getEntityComponent: coordinates out of bounds");
		}
		return ComponentManager::getComponent<Component>(m_matrix[x + y * Simulation::WIDTH]);
	}

private:
	//-------------------------------------------
  // Chunk helper functions
  //-------------------------------------------
	void activateNeighboringChunks(const int chunk_x, const int chunk_y);

	bool isValidChunk(const int chunk_x, const int chunk_y) const;

	//-------------------------------------------
  // Member Variables
  //-------------------------------------------
	std::array<entt::entity, Simulation::WIDTH * Simulation::HEIGHT> m_matrix;
	
	std::array<Chunk, Chunks::CHUNKS_X * Chunks::CHUNKS_Y> m_chunks;

	std::array<Uint32, Simulation::WIDTH * Simulation::HEIGHT> m_texture_pixels;
	
	SDL_Texture* m_simulation_texture;
	
	bool m_debug_mode;

	//-------------------------------------------
  // Static Variables
  //-------------------------------------------
	static bool s_matrix_step;
};


#endif // MATRIX_HPP