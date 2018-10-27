#pragma once
#include <iostream>
#include <string>
#include <array>

#include "Block.h"
#include "GLTexture.h"

class TexturePack
{
public:
	TexturePack() : textures(), paths(static_cast<size_t>(Block::Type::COUNT), std::string()) {}
	void init(const std::string &path);
	void apply();
private:
	GLTextureArray textures;
	std::vector<std::string> paths;
};
