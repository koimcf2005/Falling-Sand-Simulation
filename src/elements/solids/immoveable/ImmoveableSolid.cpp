#include "ImmoveableSolid.hpp"

// ------------------------------------------
// ImmoveableSolid.cpp
// ------------------------------------------
// Implementation file for ImmoveableSolid.
// This file is intentionally left empty because ImmoveableSolid
// is an abstract/marker class with no additional logic or data.
// If you add virtual methods to ImmoveableSolid in the future,
// implement them here.
// ------------------------------------------

// No-op update: Stone does not move or change
void ImmoveableSolid::update(CellularMatrix&, int, int) {
	return;
}