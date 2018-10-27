#pragma once
#include <glm/glm.hpp>
#include <iostream>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 texCoord;
};

namespace Block
{
	const glm::fvec3 vertexPositions[8]
	{
		glm::fvec3(-0.5f, 0.5f, 0.5f),
		glm::fvec3(-0.5f, -0.5f, 0.5f),
		glm::fvec3(0.5f, -0.5f, 0.5f),
		glm::fvec3(0.5f, 0.5f, 0.5f),
		glm::fvec3(-0.5f, 0.5f, -0.5f),
		glm::fvec3(-0.5f, -0.5f, -0.5f),
		glm::fvec3(0.5f, -0.5f, -0.5f),
		glm::fvec3(0.5f, 0.5f, -0.5f)
	};

	const glm::fvec3 centerPointsOnFace[6]
	{
		glm::fvec3(0.0f, 0.5f, 0.5f),  //right
		glm::fvec3(1.0f, 0.5f, 0.5f), //left
		glm::fvec3(0.5f, 1.0f, 0.5f), //top
		glm::fvec3(0.5f, 0.0f, 0.5f), //bottom
		glm::fvec3(0.5f, 0.5f, 0.0f), //front
		glm::fvec3(0.5f, 0.5f, 1.0f) //back
	};

	const unsigned short vertexIndices[6][6]
	{
		//every face with two triangles
		//right
		3,2,6,
		6,7,3,
		//left
		4,5,1,
		1,0,4,
		//up
		4,0,3,
		3,7,4,
		//down
		1,5,6,
		6,2,1,
		//front
		0,1,2,
		2,3,0,
		//back
		7,6,5,
		5,4,7
	};

	const unsigned short vertexLineIndices[48]
	{
		//right
		3, 2,
		2, 6,
		6, 7,
		7, 3,
		//left
		4, 5,
		5, 1,
		1, 0,
		0, 4,
		//up
		4, 0,
		0, 3,
		3, 7,
		7, 4,
		//down
		1, 5,
		5, 6,
		6, 2,
		2, 1,
		//front
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		//back
		7, 6,
		6, 5,
		5, 4,
		4, 7
	};

	const glm::ivec3 vertexNormals[6]
	{
		glm::ivec3(1, 0, 0),
		glm::ivec3(-1, 0, 0),
		glm::ivec3(0, 1, 0),
		glm::ivec3(0,-1, 0),
		glm::ivec3(0, 0, 1),
		glm::ivec3(0, 0,-1)
	};

	const glm::ivec2 vertexTexCoords[6]
	{
		//y-value has been inversed for adapting openGL texcord system
		glm::ivec2(0,0),
		glm::ivec2(0,1),
		glm::ivec2(1,1),
		glm::ivec2(1,1),
		glm::ivec2(1,0),
		glm::ivec2(0,0)
	};

	//block-scale typedef
	typedef unsigned short Position;
	typedef long long GlobalPosition;
	typedef glm::i16vec3 PosVec;
	typedef glm::i64vec3 GlobalPosVec;
	typedef unsigned short TypeInt;

	enum class Type: TypeInt
	{
		AIR = 0,
		DIRT,
		COBBLESTONE,
		GRASS,
		BEDROCK,
		SAND,
		WATER,
		LEAVES,
		BARK,
		COUNT,
		NON_EXIST = 255
	};


	enum class Face
	{
		RIGHT = 0,
		LEFT,
		TOP,
		BOTTOM,
		FRONT,
		BACK
	};

	constexpr bool isPermeableBlock(const Type t)
	{
		return (t == Type::AIR) || (t == Type::WATER);
	}

	constexpr bool isVisibleBlock(const Type t)
	{
		return (t != Type::AIR);
	}
};