#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "src/elements/ElementTypes.hpp"

#include <SDL2/SDL.h>
#include <string>

#include <cstdint>

enum MovementFlags : uint8_t {
  WasMoving      = 1 << 0,
  IsMoving       = 1 << 1,
	MovedThisFrame = 1 << 2
};

struct MovementState {
  uint8_t flags = 0;

  void set(MovementFlags f) { flags |= f; }
  void clear(MovementFlags f) { flags &= ~f; }
  bool has(MovementFlags f) const { return (flags & f) != 0; }

  void setMoving(bool value) {
    if (value) set(IsMoving);
    else clear(IsMoving);
  }

  void setWasMoving(bool value) {
    if (value) set(WasMoving);
    else clear(WasMoving);
  }

  void setMovedThisFrame(bool value) {
    if (value) set(MovedThisFrame);
    else clear(MovedThisFrame);
  }

  bool isMoving() const { return has(IsMoving); }
  bool wasMoving() const { return has(WasMoving); }
	bool movedThisFrame() const { return has(MovedThisFrame); }
};


/**
 * @brief Velocity component for ECS entities.
 * @param vx Velocity in X direction (float)
 * @param vy Velocity in Y direction (float)
 * @param ax Acceleration in X direction (float)
 * @param ay Acceleration in Y direction (float)
 */
struct Velocity {
	float vx; ///< Velocity X
	float vy; ///< Velocity Y
	float ax; ///< Acceleration X
	float ay; ///< Acceleration Y

	Velocity() = default;
	Velocity(const float vx, const float vy)
	: vx(vx),
		vy(vy),
		ax(0.0f),
		ay(0.0f)
	{}
};


/**
 * @brief Health component for ECS entities.
 * @param max Maximum health (int)
 * @param health Current health (int)
 */
struct Health {
	const int max;  ///< Maximum health
	int health;   ///< Current health

	Health() = default;
	Health(const int max_health)
	: max(max_health),
		health(max_health)
	{}
	Health(const int max_health, const int current_health)
	: max(max_health),
		health(current_health)
	{}

	operator int() { return health; }
	operator int() const { return health; }
	Health& operator=(float h) { health = h; return *this; }
};


/**
 * @brief Temperature component for ECS entities.
 * @param threshold Threshold for temperature effects (float)
 * @param temperature Current temperature (float)
 */
struct Temperature {
	const float threshold; ///< Threshold for effects
	float temperature;   ///< Current temperature

	Temperature() = default;
	Temperature(const float temperature_threshold)
		: threshold(temperature_threshold),
		temperature(22.0f)
	{}
	Temperature(const float temperature_threshold, const float current_temperature)
		: threshold(temperature_threshold),
		temperature(current_temperature)
	{}

	operator float() { return temperature; }
	operator float() const { return temperature; }
	Temperature& operator=(float t) { temperature = t; return *this; }
};


/**
 * @brief Element component for ECS entities.
 * @param type The element type (ElementType)
 * @param color The current color (SDL_Color)
 * @param original_color The original color (SDL_Color)
 * @param step Step flag for simulation (bool)
 */
struct Element {
	ElementType type; ///< The element type
	SDL_Color color; ///< The current color
	SDL_Color original_color; ///< The original color
	bool step; ///< Step flag for simulation
};

#endif // COMPONENTS_HPP