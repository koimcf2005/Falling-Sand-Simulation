// EmptyElement.cpp
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
// Implements the Empty element's initialization and update functions for the
// simulation. Used to define Empty-specific behavior.

#include "falling-sand-sim/elements/types/EmptyElement.hpp"

#include "falling-sand-sim/components/EnTTManager.hpp"
#include "falling-sand-sim/components/Components.hpp"
#include "falling-sand-sim/world/Matrix.hpp"

void Empty::initialize(entt::entity) {

}

void Empty::update(Matrix&, entt::entity, const int, const int) {

}