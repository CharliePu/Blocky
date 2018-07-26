#pragma once
#include <glm/glm.hpp>

#include "World.h"

extern World aWorld;

bool collide(const glm::vec3 &position)
{
	if (position.y >= Chunk::sizeY) return false;

	auto &chunk(aWorld.getCurrentChunk());
	glm::vec3 localPos;
	localPos.x = position.x - Chunk::sizeX * static_cast<int>(std::floor(position.x / Chunk::sizeX));
	localPos.y = position.y;
	localPos.z = position.z - Chunk::sizeZ * static_cast<int>(std::floor(position.z / Chunk::sizeZ));
}