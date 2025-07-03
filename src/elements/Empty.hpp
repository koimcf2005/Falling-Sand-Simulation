#ifndef EMPTY_HPP
#define EMPTY_HPP

#include "Element.hpp"

class Empty : public Element {
	public:
		Empty(int x, int y) : Element(EMPTY, x, y) {}

		void update(IMatrix&, int, int) override {
			return;
		}
};

#endif // EMPTY_HPP
