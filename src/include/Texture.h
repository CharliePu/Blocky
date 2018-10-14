#pragma once
#include <iostream>
#include <string>
#include <array>

#include "Block.h"
#include "GLTexture.h"

namespace Texture
{
	//construct it after GL is initialized
	void init(const std::string &path);
	
	GLTexture get(Block::Type type);
	glm::vec2 getTextureAtlasCoords(const unsigned short & subTextureID, const glm::vec2 & subTexCoords);
};
