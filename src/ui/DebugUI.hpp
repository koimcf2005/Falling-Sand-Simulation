// src/ui/ElementUI.hpp
#ifndef DEBUG_UI_HPP
#define DEBUG_UI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

class DebugUI {
public:
	DebugUI();
	~DebugUI();

	bool initialize(SDL_Renderer* r);

	/** Call once per frame. */
	void update(Uint32 currentTime,
				int	activeChunks,
				int	totalChunks);

	/** Call after all other rendering. */
	void render(bool debugEnabled);

	// UI layout constants
	static constexpr int FONT_SIZE = 16;
	static constexpr int TEXT_POS_X = 10;
	static constexpr int TEXT_POS_Y = 10;

private:
	void rebuildTextTexture(const std::string& text);

	SDL_Renderer* mRenderer {nullptr};
	TTF_Font* mFont {nullptr};
	SDL_Texture* mTextTexture {nullptr};
	SDL_Rect mDstRect {0, 0, 0, 0};

	Uint32 mFpsLastTime {0};
	int mFrameCount {0};
	float mFps {0.f};
};

#endif // ELEMENT_UI_HPP