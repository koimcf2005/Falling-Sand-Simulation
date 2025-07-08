# Falling Sand Simulation

A cellular automata-based particle simulation built with C++ and SDL2. This project simulates particle physics with elements like sand and stone, featuring realistic falling behavior and particle interactions.

## Features

- Real-time particle simulation  
- Multiple element types:
  - Sand (falls and spreads)
  - Dirt (falls and spreads)
  - Coal (falls and spreads)
  - Salt (falls and spreads, dissolves in water)
  - Stone (static blocks)
  - Wood (static blocks)
  - Water (falls and dissipates)
  - Oil (falls and dissipates)
  - Smoke (rises)
  - Steam (rises)
  - Empty space (air)
- Dynamic particle placement with adjustable brush size  
- Efficient rendering using hardware-accelerated textures  
- Fixed timestep physics updates  
- Randomized particle behavior for natural-looking movement  

## Controls

- **Left Mouse Button**: Place selected element  
- **Right Mouse Button**: Erase (place empty space)  
- **Mouse Wheel**: Adjust brush size (1-10)  

## Building

### Prerequisites

- C++17 compatible compiler  
- SDL2, SDL2_image, and SDL2_ttf development libraries  
- Make  

### Installing SDL2, SDL2_image, and SDL2_ttf

#### On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

#### On macOS (using Homebrew):

```bash
brew install sdl2 sdl2_image sdl2_ttf
```

#### On Windows:

1. Download the development libraries from:
   - [SDL2](https://www.libsdl.org/download-2.0.php)
   - [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
   - [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
2. Extract the files and follow the setup instructions for your compiler/IDE (e.g., Visual Studio, MinGW).
3. Ensure that the include and lib paths for all three SDL libraries are correctly referenced in your build setup.

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/koimcf2005/Falling-Sand-Simulation.git
cd Falling-Sand-Simulation

# Build the project
make
```

### Running

```bash
./build/run
```

## Technical Details

### Architecture

The simulation is built around three main components:

1. **Element System**
   - Base `Element` class with derived particle types
   - Each element type implements its own physics behavior
   - Utilizes polymorphism for element-specific updates

2. **Cellular Matrix**
   - Manages the 2D grid of elements
   - Handles element creation, deletion, and updates
   - Implements efficient rendering using SDL textures

3. **Physics System**
   - Fixed timestep updates (120Hz)
   - Bottom-up update order for proper gravity simulation
   - Randomized column updates to prevent bias

## Dependencies

- SDL2 (Simple DirectMedia Layer 2.0)  
- SDL2_image (for image loading)  
- SDL2_ttf (for font rendering)  
- C++ Standard Library  
- Standard Template Library (STL)  
