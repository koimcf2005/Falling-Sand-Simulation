#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <SDL2/SDL.h>

class Chunk {
public:	
	Chunk(const int chunk_x, const int chunk_y);
	Chunk();
	~Chunk() = default;

	int getChunkX() const;
	int getChunkY() const;

	int getLeftX() const;
	int getRightX() const;
	
	int getTopY() const;
	int getBottomY() const;

	void addNewPosition(const int x, const int y);

	void commitUpdateRect();

	const SDL_Rect& getCurrentUpdateRect() const;
	const SDL_Rect& getLastUpdateRect() const;

private:
	int m_chunk_x, m_chunk_y;
	int m_left_x, m_right_x;
	int m_top_y, m_bottom_y;
	SDL_Rect m_current_update_rect;
	SDL_Rect m_last_update_rect;
	SDL_Rect m_next_update_rect;
	bool updated_rect;
};

#endif // CHUNK_HPP