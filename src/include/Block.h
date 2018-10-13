#pragma once
#include <glm/glm.hpp>
#include <iostream>

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
	static const glm::fvec3 Block::centerPointsOnFace[6];
	static const unsigned short vertexIndices[6][6];
	static const unsigned short Block::vertexLineIndices[48];
	static const glm::ivec3 vertexNormals[6];
	static const glm::ivec2 vertexTexCoords[6];

	//block-scale typedef
	typedef unsigned short Position;
	typedef long long GlobalPosition;
	typedef glm::i16vec3 PosVec;
	typedef glm::i64vec3 GlobalPosVec;

	typedef enum
	{
		AIR = 0,
		DIRT = 1,
		COBBLESTONE = 2,
		GRASS = 3,
		BEDROCK = 4,
		SAND = 5,
		WATER = 6,
		LEAVES = 7,
		BARK = 8,
		NON_EXIST = 255
	} Type;

	typedef enum
	{
		RIGHT = 0,
		LEFT,
		TOP,
		BOTTOM,
		FRONT,
		BACK
	} Face;

	static const unsigned short Block::typeNum = 9;
};