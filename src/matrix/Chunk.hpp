#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <SDL2/SDL.h>

class Chunk {
public:	
	Chunk(const int chunk_x, const int chunk_y);
	Chunk();
	~Chunk() = default;
	
	bool wasActive() const;
	bool isActive() const;
	bool isActiveNextFrame() const;

	void activate();
	void deactivate();
	
	void activateNextFrame();
	void deactivateNextFrame();

	int getChunkX() const;
	int getChunkY() const;

	int getLeftX() const;
	int getRightX() const;
	
	int getTopY() const;
	int getBottomY() const;

	void updateRect(const int x, const int y);

	void resetDirtyRect();

	const SDL_Rect& getDirtyRect() const;
	const SDL_Rect& getOldDirtyRect() const;

	bool updateActivityState();

private:
	int m_chunk_x, m_chunk_y;
	int m_left_x, m_right_x;
	int m_top_y, m_bottom_y;
	SDL_Rect m_dirty_rect;
	SDL_Rect m_old_dirty_rect;
	bool updated_rect;
	bool m_active;
	bool m_was_active;
	bool m_active_next_frame;
};

#endif // CHUNK_HPP