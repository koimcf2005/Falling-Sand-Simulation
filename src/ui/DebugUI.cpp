#include "src/ui/DebugUI.hpp"
#include <iostream>

//------------------------------------------------------------------------------
// Constructor/Destructor and Initialization
//------------------------------------------------------------------------------

DebugUI::DebugUI(SDL_Renderer* renderer)
	: mp_Renderer(renderer)
{}

DebugUI::~DebugUI() {
	// Clean up texture and font
	if (mp_TextTexture)
		SDL_DestroyTexture(mp_TextTexture);
	if (mp_Font)
		TTF_CloseFont(mp_Font);
}

bool DebugUI::initialize() {
	// Load font for debug text
	mp_Font = TTF_OpenFont("assets/fonts/PublicPixel.ttf", FONT_SIZE);
	if (!mp_Font) {
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
// Update and Render
//------------------------------------------------------------------------------

void DebugUI::update(Uint32 currentTime, int activeChunks, int totalChunks) {
	// Update FPS and chunk stats every 250ms
	++m_FrameCount;

	if (currentTime - m_FpsLastTime >= 250) {
		m_Fps = m_FrameCount * 1000.f /
			   static_cast<float>(currentTime - m_FpsLastTime);

		m_FrameCount = 0;
		m_FpsLastTime = currentTime;

		std::string txt = "FPS: " + std::to_string(static_cast<int>(m_Fps)) +
						  "\nChunks: " +
						  std::to_string(activeChunks) + "/" +
						  std::to_string(totalChunks) + " " +
						  std::to_string(100 * activeChunks / totalChunks) + "%";

		rebuildTextTexture(txt);
	}
}

void DebugUI::render(bool debugEnabled) {
	// Render the debug overlay if enabled and texture is valid
	if (!debugEnabled || !mp_TextTexture)
		return;
	SDL_RenderCopy(mp_Renderer, mp_TextTexture, nullptr, &m_DstRect);
}

//------------------------------------------------------------------------------
// Internal Helpers
//------------------------------------------------------------------------------

void DebugUI::rebuildTextTexture(const std::string &text) {
	// Rebuild the debug text texture from the given string
	if (!mp_Font)
		return;

	if (mp_TextTexture) {
		SDL_DestroyTexture(mp_TextTexture);
		mp_TextTexture = nullptr;
	}

	SDL_Color color{255, 255, 255, 255}; // white
	SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(mp_Font, text.c_str(), color, 800);
	if (!surf)
		return;

	mp_TextTexture = SDL_CreateTextureFromSurface(mp_Renderer, surf);
	if (mp_TextTexture) {
		m_DstRect = {TEXT_POS_X, TEXT_POS_Y, surf->w, surf->h};
	}
	SDL_FreeSurface(surf);
}