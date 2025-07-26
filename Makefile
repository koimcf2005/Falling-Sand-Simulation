# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++20 -Wextra -MMD -MP -I/usr/include/SDL2 -I. -march=native -flto -funroll-loops
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIRS = -Iinclude -Ilibs/entt

# Sources and objects
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

# Output
TARGET = $(BUILD_DIR)/run

# Rules
all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(TARGET): | $(BUILD_DIR)
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

# Include dependency files if they exist
-include $(DEPS)
