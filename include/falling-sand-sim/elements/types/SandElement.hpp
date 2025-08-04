// SandElement.hpp
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
// Declares the Sand element's initialization and update functions for the
// simulation. Used to define Sand-specific behavior.

#ifndef SAND_ELEMENT_HPP
#define SAND_ELEMENT_HPP

#include <entt/entt.hpp>

class Matrix;

namespace Sand {
	void initialize(entt::entity entity);
	void update(Matrix& matrix, entt::entity entity, const int x, const int y);
}

#endif // SAND_ELEMENT_HPP