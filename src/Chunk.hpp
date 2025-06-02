#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <utility>

class Chunk {
public:
	Chunk(std::pair<int, int> bottomRight, std::pair<int, int> topLeft) 
		: bottomRight(bottomRight),
		topLeft(topLeft)
	{}

	std::pair<int, int> getBottomRight() { return bottomRight; }
	void setBottomRight(std::pair<int, int> point) { bottomRight = point; }

	std::pair<int, int> getTopLeft() { return topLeft; }
	void setTopLeft(std::pair<int, int> point) { topLeft = point; }

	bool getshouldUpdate() { return shouldUpdate; }
	void setshouldUpdate(bool value) { shouldUpdate = value; }

	bool getShouldUpdateNextFrame() { return shouldUpdateNextFrame; }
	void setShouldUpdateNextFrame(bool value) { shouldUpdateNextFrame = value; }

private:
	bool shouldUpdate = true;
	bool shouldUpdateNextFrame = true;

	std::pair<int, int> bottomRight;
	std::pair<int, int> topLeft;
};

#endif