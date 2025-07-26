#ifndef SAND_ELEMENT_HPP
#define SAND_ELEMENT_HPP

#include <entt/entt.hpp>

class Matrix;

namespace Sand {
	void initialize(entt::entity entity);
	void update(Matrix& matrix, entt::entity entity);
}

#endif // SAND_ELEMENT_HPP