#ifndef SIMULATION_TEXTURE_HPP
#define SIMULATION_TEXTURE_HPP

#include "src/matrix/Matrix.hpp"

#include <SDL2/SDL.h>

class SimulationTexture {
public:
  SDL_Texture* getTexture() const;
  void initializeTexture(SDL_Renderer* renderer);
  void updateTexture(const Matrix& matrix);

  void toggleShowChunks();

private:
  SDL_Texture* m_simulation_texture;
    
  bool m_show_chunks;

  bool m_debug_mode;

};

#endif // SIMULATION_TEXTURE_HPP