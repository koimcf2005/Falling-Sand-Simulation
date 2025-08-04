// SystemStatsDisplay.cpp
// Copyright (C) 2025 Koi McFarland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Author: koimcf168@gmail.com
//
// Implements the SystemStatsDisplay for rendering FPS, TPS, memory, and
// entity/component stats overlay in the simulation.

#include "falling-sand-sim/renderer/SystemStatsDisplay.hpp"

#include "falling-sand-sim/components/EnTTManager.hpp"
#include "falling-sand-sim/components/Components.hpp"
#include "falling-sand-sim/renderer/Renderer.hpp"

#include <sys/times.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#elif __APPLE__
    #include <mach/mach.h>
#else // Linux
    #include <unistd.h>
#endif

TTF_Font* SystemStatsDisplay::sp_font{nullptr};
SDL_Texture* SystemStatsDisplay::sp_text_texture{nullptr};
SDL_Rect SystemStatsDisplay::s_destination_rect{0, 0, 0, 0};
Uint32 SystemStatsDisplay::s_fps_last_time{0};
int SystemStatsDisplay::s_frame_count{0};
float SystemStatsDisplay::s_fps{0.0f};
bool SystemStatsDisplay::s_is_active(false);
Uint32 SystemStatsDisplay::s_tick_last_time{0};
int SystemStatsDisplay::s_tick_count{0};
float SystemStatsDisplay::s_tick_rate{0.0f};

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

bool SystemStatsDisplay::initialize() {
  // Load font for debug text
  sp_font = TTF_OpenFont("assets/fonts/Monaco.ttf", FONT_SIZE);
  if (!sp_font) {
    std::cerr << "Failed to load font: " << TTF_GetError() << '\n';
    return false;
  }
  return true;
}

void SystemStatsDisplay::incrementFrameCount() {
  ++s_frame_count;
}

void SystemStatsDisplay::incrementTickCount() {
  ++s_tick_count;
}

void SystemStatsDisplay::update(const Uint32 currentTime, const bool isActive) {
  s_is_active = isActive;
  if (!s_is_active) return;

  int entity_count = EnTTManager::getRegistry().storage<entt::entity>().size();
  int componentCount = EnTTManager::getRegistry().storage<Element>().size()
                     + EnTTManager::getRegistry().storage<Velocity>().size()
                     + EnTTManager::getRegistry().storage<Health>().size()
                     + EnTTManager::getRegistry().storage<Temperature>().size();

  // Update FPS and tick stats every 100ms
  ++s_frame_count;

  if (currentTime - s_fps_last_time >= 100) {
    s_fps = s_frame_count * 1000.f /
         static_cast<float>(currentTime - s_fps_last_time);
    s_frame_count = 0;
    s_fps_last_time = currentTime;

    // Tick rate calculation
    Uint32 tick_elapsed = currentTime - s_tick_last_time;
    if (tick_elapsed > 0) {
      s_tick_rate = s_tick_count * 1000.f / static_cast<float>(tick_elapsed);
      s_tick_count = 0;
      s_tick_last_time = currentTime;
    }

    int memMB = getProcessMemoryMB();

    std::string txt =
              "\nFPS: " + std::to_string(static_cast<int>(s_fps)) +
              "\nTPS: " + std::to_string(static_cast<int>(s_tick_rate)) +
              "\nMem: " + std::to_string(memMB) + "MB" +
              "\nEnt: " + std::to_string(entity_count) +
              "\nCmp: " + std::to_string(componentCount);

    rebuildTextTexture(txt);
  }
}

void SystemStatsDisplay::render() {
  // Render the debug overlay if enabled and texture is valid
  if (!s_is_active || !sp_text_texture)
    return;
  SDL_RenderCopy(Renderer::getRenderer(), sp_text_texture, nullptr, &s_destination_rect);
}

void SystemStatsDisplay::rebuildTextTexture(const std::string &text) {
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