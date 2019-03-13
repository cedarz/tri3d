#pragma once
#include <vector>
#include <memory>
class Renderer {
public:
	Renderer(int screen_width, int screen_height, unsigned int * frame_buffer) : 
		screen_width_(screen_width), screen_height_(screen_height), frame_buffer_(frame_buffer) {}
	void render(unsigned int * fb);

private:
	int screen_width_;
	int screen_height_;
	unsigned int * frame_buffer_;

	std::shared_ptr<std::vector<unsigned char>> frame_;
};