#ifndef EMPTY_ELEMENT_HPP
#define EMPTY_ELEMENT_HPP

#include <entt/entt.hpp>

class Matrix;

namespace Empty {
	void initialize(entt::entity entity);
	void update(Matrix& matrix, entt::entity entity);
}

#endif // EMPTY_ELEMENT_HPP