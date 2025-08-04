// Matrix.hpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// Declares the Matrix class, which manages the simulation grid, element
// placement, updates, and chunk management.

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "falling-sand-sim/core/Globals.hpp"
#include "falling-sand-sim/world/Chunk.hpp"
#include "falling-sand-sim/elements/ElementTypes.hpp"
#include "falling-sand-sim/components/Components.hpp"
#include "falling-sand-sim/components/EnTTManager.hpp"

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
	~Matrix() = default;

	//-------------------------------------------
	// Main update loop
	//-------------------------------------------
	void update();
	void updateCellByCell(int step_count = 1);

	//-------------------------------------------
	// Element Management 
	//-------------------------------------------
	bool isInBounds(const int x, const int y) const;

	bool isEmpty(const int x, const int y) const;

	void placeElement(const ElementType type, const int x, const int y);
	void placeElementsInArea(const ElementType type, const int x, const int y, const int radius);

	void swapEntities(const int x1, const int y1, const int x2, const int y2);

	void updateChunk(const int x, const int y);

	//-------------------------------------------
  // Rendering and Debug 
  //-------------------------------------------
	void toggleDebugMode();
	
	//-------------------------------------------
  // Global Static Step
  //-------------------------------------------
	static bool getStep();
	static int getDebugIndex();

	//-------------------------------------------
  // Chunk Getters
  //-------------------------------------------
	int getChunkX(const int x) const;
	int getChunkY(const int y) const;

	Chunk& getChunk(const int chunk_x, const int chunk_y);
	const Chunk& getChunk(const int chunk_x, const int chunk_y) const;

	const std::array<Chunk, Chunks::CHUNKS_X * Chunks::CHUNKS_Y>& getChunks() const;

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
		return EnTTManager::hasComponent<Component>(getEntity(x, y));
	}

	template<typename Component>
	Component& getEntityComponent(const int x, const int y) {
		if (!isInBounds(x, y)) {
			throw std::out_of_range("Matrix::getEntityComponent: coordinates out of bounds");
		}
		return EnTTManager::getComponent<Component>(m_matrix[x + y * Simulation::WIDTH]);
	}

	template<typename Component>
	Component& getEntityComponent(const int x, const int y) const {
		if (!isInBounds(x, y)) {
			throw std::out_of_range("Matrix::getEntityComponent: coordinates out of bounds");
		}
		return EnTTManager::getComponent<Component>(m_matrix[x + y * Simulation::WIDTH]);
	}

private:
	//-------------------------------------------
	// Chunk helper functions
	//-------------------------------------------
	bool isValidChunk(const int chunk_x, const int chunk_y) const;

	//-------------------------------------------
	// Member Variables
	//-------------------------------------------
	std::array<entt::entity, Simulation::WIDTH * Simulation::HEIGHT> m_matrix;

	std::array<Chunk, Chunks::CHUNKS_X * Chunks::CHUNKS_Y> m_chunks;

	bool m_debug_mode;

	//-------------------------------------------
	// Static Variables
	//-------------------------------------------
	static bool s_matrix_step;

	static int s_debug_index;
};


#endif // MATRIX_HPP