#include "src/renderer/ui/DebugUI.hpp"

#include "src/renderer/Renderer.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Components.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/times.h>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#elif __APPLE__
    #include <mach/mach.h>
#else // Linux
    #include <unistd.h>
#endif

TTF_Font* DebugUI::sp_font{nullptr};
SDL_Texture* DebugUI::sp_text_texture{nullptr};
SDL_Rect DebugUI::s_destination_rect{0, 0, 0, 0};
Uint32 DebugUI::s_fps_last_time{0};
int DebugUI::s_frame_count{0};
float DebugUI::s_fps{0.0f};
bool DebugUI::s_is_active(false);

// Helper to get process memory usage (RSS, in MB)
static int getProcessMemoryMB() {
#ifdef _WIN32
  PROCESS_MEMORY_COUNTERS pmc;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    return static_cast<int>(pmc.WorkingSetSize / (1024 * 1024));
  }
  return -1;
#elif __APPLE__
  mach_task_basic_info info;
  mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) == KERN_SUCCESS) {
    return static_cast<int>(info.resident_size / (1024 * 1024));
  }
  return -1;
#else // Linux
  std::ifstream status("/proc/self/status");
  std::string line;
  int rss = 0;
  while (std::getline(status, line)) {
    if (line.find("VmRSS:") == 0) {
      std::istringstream iss(line);
      std::string key;
      int value;
      std::string unit;
      iss >> key >> value >> unit;
      rss = value; // kB
      break;
    }
  }
  return rss / 1024; // MB
#endif
}

bool DebugUI::initialize() {
  // Load font for debug text
  sp_font = TTF_OpenFont("assets/fonts/Monaco.ttf", FONT_SIZE);
  if (!sp_font) {
    std::cerr << "Failed to load font: " << TTF_GetError() << '\n';
    return false;
  }
  return true;
}

void DebugUI::update(const Uint32 currentTime, const int activeChunks, const int totalChunks, const bool isActive) {
  s_is_active = isActive;
  if (!s_is_active) return;
  
  int entity_count = ComponentManager::getRegistry().storage<entt::entity>().size();
  int componentCount = ComponentManager::getRegistry().storage<Element>().size()
                     + ComponentManager::getRegistry().storage<Velocity>().size()
                     + ComponentManager::getRegistry().storage<Health>().size()
                     + ComponentManager::getRegistry().storage<Temperature>().size();

  // Update FPS and chunk stats every 100s
  ++s_frame_count;

  if (currentTime - s_fps_last_time >= 100) {
    s_fps = s_frame_count * 1000.f /
         static_cast<float>(currentTime - s_fps_last_time);

    s_frame_count = 0;
    s_fps_last_time = currentTime;

    int memMB = getProcessMemoryMB();

    std::string txt = "Chunks: " +
              std::to_string(activeChunks) + "/" +
              std::to_string(totalChunks) + " " +
              std::to_string(static_cast<int>(100.0f * activeChunks / totalChunks)) + "%" +
              "\nFPS: " + std::to_string(static_cast<int>(s_fps)) +
              "\nMem: " + std::to_string(memMB) + "MB" +
              "\nEnt: " + std::to_string(entity_count) +
              "\nCmp: " + std::to_string(componentCount);

    rebuildTextTexture(txt);
  }
}

void DebugUI::render() {
  // Render the debug overlay if enabled and texture is valid
  if (!s_is_active || !sp_text_texture)
    return;
  SDL_RenderCopy(Renderer::getRenderer(), sp_text_texture, nullptr, &s_destination_rect);
}

void DebugUI::rebuildTextTexture(const std::string &text) {
  // Rebuild the debug text texture from the given string
  if (!sp_font)
    return;

  if (sp_text_texture) {
    SDL_DestroyTexture(sp_text_texture);
    sp_text_texture = nullptr;
  }

  // Change font color here (example: green)
  SDL_Color color{0, 255, 0, 255}; // green
  SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(sp_font, text.c_str(), color, 800);
  if (!surf)
    return;

  sp_text_texture = SDL_CreateTextureFromSurface(Renderer::getRenderer(), surf);
  if (sp_text_texture) {
    s_destination_rect = {TEXT_POS_X, TEXT_POS_Y, surf->w, surf->h};
  }
  SDL_FreeSurface(surf);
}