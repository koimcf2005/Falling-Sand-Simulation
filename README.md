# Falling Sand Simulation

A cellular automata-based particle simulation built with C++, EnTT, and SDL2. This project creates a physics simulation for every pixel on a 512x512 grid using an Entity Component System.

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
  - Fire (Consumes elements like wood and oil)
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

## Python Build Script

If you prefer a fully automated build experience, use the `build.py` script:

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

## Building Without Python

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

## Dependencies

- **C++17** — Modern language features  
- **EnTT** — ECS (Entity-Component-System) architecture (included in `libs/entt`)
- **SDL2** — Rendering, input, windowing  
- **SDL2_image** — PNG loading  
- **SDL2_ttf** — Font rendering  
- **CMake** — Build configuration  
- **Python 3** — Cross-platform build management via `build.py`  

---

## License

This project is licensed under the terms of the [GNU Affero General Public License v3.0](https://www.gnu.org/licenses/agpl-3.0.html).

© 2024 Koi McFarland. You may use, modify, and distribute this software freely under AGPL-3.0, but **commercial use is strictly prohibited unless you also open-source your derivative work under the same license.**

This project uses open source libraries (EnTT and SDL2). See their respective licenses in the `include/` folder.