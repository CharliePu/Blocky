#pragma once
#include <iostream>
#include <string>
#include <array>

#include "Block.h"
#include "GLTexture.h"

class Texture
{
public:
	//construct it after GL is initialized
	static void init(const std::string &path);
	
	static GLTexture get(Block::Type type);
	static GLTexture get(decltype(Block::typeNum) type);
private:
	static std::array<GLTexture, Block::typeNum> textures;
};

inline GLTexture Texture::get(Block::Type type)
{
	return textures[static_cast<size_t>(type)];
}

inline GLTexture Texture::get(decltype(Block::typeNum) type)
{
	return textures[static_cast<decltype(Block::typeNum)>(type)];
}
