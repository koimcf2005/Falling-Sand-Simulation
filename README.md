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

---

## Build System

This project uses **CMake** and a **cross-platform Python script (`build.py`)** to manage builds and execution. It supports both Debug and Release builds and works on **Linux**, **macOS**, and **Windows** (with Python and CMake installed).

### Prerequisites

- Python 3  
- CMake 3.19+  
- A C++17 compatible compiler  
- SDL2, SDL2_image, and SDL2_ttf development libraries

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

1. Install [CMake](https://cmake.org/download/) and [Python](https://www.python.org/downloads/)
2. Use [vcpkg](https://github.com/microsoft/vcpkg) to install SDL2 libraries:
   ```bash
   ./vcpkg install sdl2 sdl2-image sdl2-ttf
   ```
3. Pass the toolchain file when configuring:
   ```bash
   cmake --preset=debug -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

---

## 🛠 Building Without Python

If you don't want to use `build.py`, you can build the project manually using CMake.

### Using CMake Presets (recommended):

```bash
# Configure and build debug
cmake --preset=debug
cmake --build --preset=debug

# OR for release
cmake --preset=release
cmake --build --preset=release
```

### Without Presets (classic method):

```bash
# Debug build
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build .

# Release build
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build .
```

This builds the binary to `build/[type]/bin/FallingSandSim`.

---

## Python Build Script

If you prefer a fully automated experience, use the `build.py` script:

### Build Debug Version

```bash
python build.py debug
```

### Build Release Version

```bash
python build.py release
```

### Run the Simulation (Debug Build)

```bash
python build.py run-debug
```

### Run the Simulation (Release Build)

```bash
python build.py run-release
```

### Clean All Build Files

```bash
python build.py clean
```

---

## Technical Architecture

### 1. Element System

- Each element inherits from a base `Element` class
- Individual behaviors are defined per element (e.g., `SandElement`)
- Behavior is polymorphic and encapsulated

### 2. Cellular Matrix

- Manages a 2D grid of elements
- Handles per-frame updates and state transitions
- Efficient rendering via SDL textures

### 3. Physics System

- Fixed timestep physics at 120Hz
- Gravity simulated with bottom-up updates
- Column update order randomized to prevent directional bias

---

## Dependencies

- **SDL2** — Rendering, input, windowing  
- **SDL2_image** — PNG loading  
- **SDL2_ttf** — Font rendering  
- **C++17** — Modern language features  
- **CMake** — Build configuration  
- **Python 3** — Cross-platform build management via `build.py`  
- **EnTT** — ECS (Entity-Component-System) architecture (included in `libs/entt`)

---

## License

This project uses open source libraries (like EnTT and SDL2). See their respective licenses in the `libs/` folder.