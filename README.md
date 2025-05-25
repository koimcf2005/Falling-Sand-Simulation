# Falling Sand Simulation

A cellular automata-based particle simulation built with C++ and SDL2. This project simulates particle physics with elements like sand and stone, featuring realistic falling behavior and particle interactions.

## Features

- Real-time particle simulation
- Multiple element types:
  - Sand (falls and spreads realistically)
  - Stone (static blocks)
  - Empty space (air)
- Dynamic particle placement with adjustable brush size
- Efficient rendering using hardware-accelerated textures
- Fixed timestep physics updates
- FPS display
- Randomized particle behavior for natural-looking movement

## Controls

- **Left Mouse Button**: Place selected element
- **Right Mouse Button**: Erase (place empty space)
- **Mouse Wheel**: Adjust brush size (1-10)
- **1**: Select Sand
- **2**: Select Stone

## Building

### Prerequisites

- C++17 compatible compiler
- SDL2 and SDL2_image development libraries
- Make

### Installing SDL2 and SDL2_image

#### On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev
```

#### On macOS (using Homebrew):

```bash
brew install sdl2 sdl2_image
```

#### On Windows:

1. Download the SDL2 and SDL2_image development libraries from:
   - [SDL2](https://www.libsdl.org/download-2.0.php)
   - [SDL2_image](https://www.libsdl.org/projects/SDL_image/)
2. Extract the files and follow the SDL2/SDL2_image setup instructions for your compiler/IDE (e.g., Visual Studio, MinGW).
3. Ensure that the include and lib paths for both SDL2 and SDL2_image are correctly referenced in your build setup.

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
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
- C++ Standard Library
- Standard Template Library (STL)
