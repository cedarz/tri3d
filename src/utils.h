#pragma once
#include <vector>
#include <exception>
#include <string>
#include <memory>

class PNGError : public std::runtime_error {
public:
	PNGError(const std::string& error) : std::runtime_error(error) {}
};

bool LoadPng(const char* filename, int& width, int& height, int& channels, 
	std::shared_ptr<std::vector<unsigned char>> & img);
