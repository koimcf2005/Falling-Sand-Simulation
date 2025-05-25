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
- SDL2 development libraries
- Make

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
./run
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
- C++ Standard Library
- Standard Template Library (STL)
