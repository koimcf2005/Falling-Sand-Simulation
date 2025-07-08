# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -std=c++17 -Wall -Wextra -MMD -MP -I/usr/include/SDL2 -I.

# Linker flags
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Project directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files: all .cpp files in src/ and subdirectories
SRC_FILES = $(shell find $(SRC_DIR) -name '*.cpp')

# Object files (in build directory, mirroring src/ structure)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Target executable name
TARGET = build/run

# Default rule
all: directories $(TARGET)

# Create build directories (mirrors src/ structure for .o files)
directories:
	@mkdir -p $(OBJ_DIR)
	@find $(SRC_DIR) -type d | sed 's|^$(SRC_DIR)|$(OBJ_DIR)|' | xargs -I{} mkdir -p {}

# Linking the final executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)

# Compile each .cpp file into a .o file (mirroring src/ structure)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include dependency files for automatic header tracking
-include $(OBJ_FILES:.o=.d)

# Clean up
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
