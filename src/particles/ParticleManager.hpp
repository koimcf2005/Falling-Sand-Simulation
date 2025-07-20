#ifndef PARTICLE_MANAGER_HPP
#define PARTICLE_MANAGER_HPP

#include <SDL2/SDL.h>
#include <array>

#include "src/core/Globals.hpp"
#include "src/elements/utilities/rng/ElementRNG.hpp"

/**
 * @brief Represents a single particle in the simulation.
 * 
 * Supports subpixel velocity, acceleration, color, fading, and lifetime.
 */
struct Particle {
	int x, y;                   ///< Integer position in simulation grid
	int width, height;          ///< Size of the particle (in cells)
	SDL_Color color;            ///< RGBA color of the particle
	float velocityX, velocityY; ///< Velocity (can be non-integer)
	float accelerationX, accelerationY; ///< Acceleration per frame
	int lifetime;               ///< Remaining lifetime (frames)
	float lifetimeRandomness;   ///< Randomness factor for initial lifetime
	float fadeThreshold;        ///< Fraction of lifetime to start fading

	float accumulationX;        ///< Accumulated subpixel X position
	float accumulationY;        ///< Accumulated subpixel Y position
	float maxLifetime;          ///< Initial lifetime (for fading)
	int alpha;                  ///< Initial alpha value

	/**
	 * @brief Default constructor. Initializes all fields to default values.
	 */
	Particle()
		: x(0), y(0), width(1), height(1),
		  color{0, 0, 0, 255},
		  velocityX(0), velocityY(0),
		  accelerationX(0), accelerationY(0),
		  lifetime(0), lifetimeRandomness(0.0f), fadeThreshold(1.0f),
		  maxLifetime(0), alpha(255)
	{}

	/**
	 * @brief Parameterized constructor for Particle.
	 * @param x X position
	 * @param y Y position
	 * @param w Width
	 * @param h Height
	 * @param c Color (SDL_Color)
	 * @param vx Initial velocity X
	 * @param vy Initial velocity Y
	 * @param ax Acceleration X per frame
	 * @param ay Acceleration Y per frame
	 * @param l Lifetime (frames)
	 * @param lr Lifetime randomness (0=no randomness, 0.2=±20%)
	 * @param ft Fade threshold (fraction of lifetime to start fading)
	 */
	Particle(
		int x, int y,
		int w, int h,
		SDL_Color c,
		float vx = 0, float vy = 0,
		float ax = 0, float ay = 0,
		int l = 100,
		float lr = 0.0f,
		float ft = 1.0f
	)
	: x(x), y(y),
	width(w), height(h),
	color(c),
	velocityX(vx), velocityY(vy),
	accelerationX(ax), accelerationY(ay),
	lifetime(l * ElementRNG::getRandomFloat(1 - lr, 1 + lr)),
	fadeThreshold(ft),
	maxLifetime(lifetime),
	alpha(color.a)
	{}
};

class ParticleManager {
public:
	static constexpr size_t s_MAX_PARTICLES = 4096;

	/**
	 * @brief Parameterized constructor for Particle.
	 * @param x X position
	 * @param y Y position
	 * @param w Width
	 * @param h Height
	 * @param c Color (SDL_Color)
	 * @param vx Initial velocity X
	 * @param vy Initial velocity Y
	 * @param ax Acceleration X per frame
	 * @param ay Acceleration Y per frame
	 * @param l Lifetime (frames)
	 * @param lr Lifetime randomness (0=no randomness, 0.2=±20%)
	 * @param ft Fade threshold (fraction of lifetime to start fading)
	 */
	static bool spawnParticle(const Particle p);

	// Remove particle at index (swap with last)
	static void removeParticle(size_t index);

	// Access active particles
	static Particle* begin();
	static Particle* end();
	static Particle* getParticle(int index);
	static size_t size();

	static void updateParticles();

private:
	static Particle m_Particles[s_MAX_PARTICLES];
	static size_t m_Count;
};


#endif // PARTICLE_MANAGER_HPP