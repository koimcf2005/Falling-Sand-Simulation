#include "src/ui/DebugUI.hpp"
#include <iostream>

// ─────────────────────────────────────────────
DebugUI::DebugUI()
	: mRenderer(nullptr)
{}

// ─────────────────────────────────────────────
DebugUI::~DebugUI() {
	if (mTextTexture)
		SDL_DestroyTexture(mTextTexture);
	if (mFont)
		TTF_CloseFont(mFont);
}

bool DebugUI::initialize(SDL_Renderer* r) {
	mRenderer = r;
	// Load font (smaller size for tooltip)
	mFont = TTF_OpenFont("assets/fonts/PublicPixel.ttf", FONT_SIZE);
	if (!mFont) {
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
		return false;
	}
	return true;
}

// ─────────────────────────────────────────────
void DebugUI::update(Uint32 currentTime, int activeChunks, int totalChunks){
	++mFrameCount;

	if (currentTime - mFpsLastTime >= 250) { // 1 s elapsed
		mFps = mFrameCount * 1000.f /
			   static_cast<float>(currentTime - mFpsLastTime);

		mFrameCount = 0;
		mFpsLastTime = currentTime;

		std::string txt = "FPS: " + std::to_string(static_cast<int>(mFps)) +
						  "\nChunks: " +
						  std::to_string(activeChunks) + "/" +
						  std::to_string(totalChunks) + " " +
						  std::to_string(100 * activeChunks / totalChunks) + "%";

		rebuildTextTexture(txt);
	}
}

// ─────────────────────────────────────────────
void DebugUI::render(bool debugEnabled)
{
	if (!debugEnabled || !mTextTexture)
		return;
	SDL_RenderCopy(mRenderer, mTextTexture, nullptr, &mDstRect);
}

// ─────────────────────────────────────────────
void DebugUI::rebuildTextTexture(const std::string &text) {
	if (!mFont)
		return;

	if (mTextTexture) { // discard old texture
		SDL_DestroyTexture(mTextTexture);
		mTextTexture = nullptr;
	}

	SDL_Color color{255, 255, 255, 255}; // white
	SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(mFont, text.c_str(), color, 800);
	if (!surf)
		return;

	mTextTexture = SDL_CreateTextureFromSurface(mRenderer, surf);
	if (mTextTexture) {
		mDstRect = {TEXT_POS_X, TEXT_POS_Y, surf->w, surf->h};
	}
	SDL_FreeSurface(surf);
}