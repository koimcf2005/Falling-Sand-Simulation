#include "src/particles/ParticleManager.hpp"
#include <iostream>

// Define static members
Particle ParticleManager::m_Particles[s_MAX_PARTICLES]{};
size_t ParticleManager::m_Count = 0;

// Add a particle, returns true if successful
bool ParticleManager::spawnParticle(const Particle p) {
	if (m_Count >= s_MAX_PARTICLES) return false;
	m_Particles[m_Count++] = p;
	return true;
}

// Remove particle at index (swap with last)
void ParticleManager::removeParticle(size_t index) {
	if (index < m_Count) {
		m_Particles[index] = m_Particles[m_Count - 1];
		--m_Count;
	}
}

// Access active particles
Particle* ParticleManager::getParticle(int index) { return &m_Particles[index]; }
size_t ParticleManager::size() { return m_Count; }

void ParticleManager::updateParticles() {
	size_t i = 0;
	while (i < m_Count) {

		Particle& p = m_Particles[i];

		// Subpixel movement and acceleration
		p.velocityX += p.accelerationX;
		p.velocityY += p.accelerationY;
		p.accumulationX += p.velocityX;
		p.accumulationY += p.velocityY;
		int moveX = static_cast<int>(p.accumulationX);
		int moveY = static_cast<int>(p.accumulationY);
		p.x += moveX;
		p.y += moveY;
		p.accumulationX -= moveX;
		p.accumulationY -= moveY;

		p.lifetime--;

		bool outOfBounds = 
			p.x + p.width  < 0 || p.x >= Matrix::WIDTH ||
			p.y + p.height < 0 || p.y >= Matrix::HEIGHT;

		float fadeThreshold = p.maxLifetime * p.fadeThreshold;
		p.color.a = static_cast<Uint8>(p.alpha *
			(std::min(static_cast<float>(p.lifetime), fadeThreshold) / fadeThreshold));

		if (p.lifetime <= 0 || outOfBounds) {
			removeParticle(i);
			// Do not increment i, process the swapped-in particle
		} else {
			++i;
		}
	}
}