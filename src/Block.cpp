#include "Block.h"

const glm::fvec3 Block::vertexPositions[]
{
	glm::fvec3(0.0f, 1.0f, 1.0f),
	glm::fvec3(0.0f, 0.0f, 1.0f),
	glm::fvec3(1.0f, 0.0f, 1.0f),
	glm::fvec3(1.0f, 1.0f, 1.0f),
	glm::fvec3(0.0f, 1.0f, 0.0f),
	glm::fvec3(0.0f, 0.0f, 0.0f),
	glm::fvec3(1.0f, 0.0f, 0.0f),
	glm::fvec3(1.0f, 1.0f, 0.0f)
};

//const glm::fvec3 Block::vertexPositions[]
//{
//	glm::fvec3(-0.5, 0.5, 0.5),
//	glm::fvec3(-0.5, -0.5, 0.5),
//	glm::fvec3(0.5, -0.5, 0.5),
//	glm::fvec3(0.5, 0.5, 0.5),
//	glm::fvec3(-0.5, 0.5, -0.5),
//	glm::fvec3(-0.5, -0.5, -0.5),
//	glm::fvec3(0.5, -0.5, -0.5),
//	glm::fvec3(0.5, 0.5, -0.5)
//};


const unsigned short Block::vertexIndices[6][6]
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

const glm::ivec3 Block::vertexNormals[6] =
{
	glm::ivec3(1, 0, 0),
	glm::ivec3(-1, 0, 0),
	glm::ivec3(0, 1, 0),
	glm::ivec3(0,-1, 0),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, 0,-1)
};

const glm::ivec2 Block::vertexTexCoords[6]
{
	//y-value has been inversed for adapting openGL texcord system
	glm::vec2(0,0),
	glm::vec2(0,1),
	glm::vec2(1,1),
	glm::vec2(1,1),
	glm::vec2(1,0),
	glm::vec2(0,0)
};