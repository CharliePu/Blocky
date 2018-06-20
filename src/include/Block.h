#pragma once
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Block
{
	//data that will be combined to vertices
	static const glm::fvec3 vertexPositions[8];
	static const unsigned short vertexIndices[6][6];
	static const glm::ivec3 vertexNormals[6];
	static const glm::ivec2 vertexTexCoords[6];

	//block-scale typedef
	typedef unsigned short Position;
	typedef long long GlobalPosition;
	typedef enum
	{
		AIR = 0,
		DIRT = 1,
		COBBLESTONE = 2,
		GRASS = 3,
		BEDROCK = 4
	} Type;
	static const unsigned short Block::typeNum = 5;
};