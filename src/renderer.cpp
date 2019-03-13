#include "renderer.h"
#include "utils.h"
#include <Windows.h>
#include <iostream>
#include <algorithm>

void Renderer::render(unsigned int * fb) {
	//char path[MAX_PATH];
	//GetCurrentDirectoryA(MAX_PATH, path);
	//std::cout << path << std::endl;
	int tex_w{ 0 };
	int tex_h{ 0 };
	int tex_ch{ 0 };
	LoadPng("../Resource/texture/flowers.png", tex_w, tex_h, tex_ch, frame_);
	for (int h = 0; h < (std::min)(screen_height_, tex_h); ++h) {
		for (int w = 0; w < (std::min)(screen_width_, tex_w); ++w) {
				//fb[h * screen_width_ * 4 + w * 4 +byte] = 255;
				unsigned int r = (*frame_)[h * tex_w * 4 + w * 4 + 0];
				unsigned int g = (*frame_)[h * tex_w * 4 + w * 4 + 1];
				unsigned int b = (*frame_)[h * tex_w * 4 + w * 4 + 2];
				unsigned int a = (*frame_)[h * tex_w * 4 + w * 4 + 3];
				unsigned int color = (a << 24) | (r << 16) | (g << 8) | (b);
				//std::cout << int(pixel) << std::endl;
				//if(w > 384) color = 0;
				fb[h * screen_width_ + w] = color;
			
		}
	}
}