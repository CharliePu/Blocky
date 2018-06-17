#pragma once
//std headers
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>

//gl headers
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\noise.hpp>

#include "Debug.h"
#include "Texture.h"
#include "Camera.h"
#include "Noise.h"

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
	typedef enum
	{
		AIR,
		DIRT,
		COBBLESTONE,
		GRASS,
		BEDROCK
	} Type;
	static const unsigned short Block::typeNum = 5;
};

struct Chunk
{
	static constexpr Block::Position sizeX = 16, sizeY = 128, sizeZ = 16;

	typedef int Position;
	typedef glm::ivec2 PosVec;
};


const Chunk::Position renderSize = 10;
const int checkInterval = 5;


//world-scale definition
typedef long long globalBlockPos;


//init renderer
void initBlockRenderer();

class World
{
public:
	World(Chunk::PosVec position): 
		chunkX(position.x), chunkZ(position.y), 
		needUpdate(false), 
		vao(0), vbo(0), 
		debugVao(0), debugVbo(0),
		data() {};

	~World();

	void generate(globalBlockPos noiseX, globalBlockPos noiseZ);
	void draw();
	void update();
	void save();
	bool load();
	void debug();
	Chunk::Position chunkX, chunkZ;
	bool needUpdate;
private:
	//enlarged for surrounding block detection
	Block::Type data[Chunk::sizeX + 2][Chunk::sizeY + 2][Chunk::sizeZ + 2];

	//split vertices by different texture
	std::vector<Vertex> verticesBuffer[Block::typeNum];

	friend Block::Type getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const World* currentChunk);
	friend Block::Type getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z);
	//rendering stuffs
	GLuint vao, vbo, debugVao, debugVbo;
	GLsizei verticesOffset[Block::typeNum + 1];//offset between vertices
	void addBlockVertices(const Block::Position & x, const Block::Position & y, const Block::Position & z, std::vector<Vertex> verticesGroups[]);
};


//render chunks
void drawWorld();
void drawWorldDebug();
void enableUpdateThread();
void disableUpdateThread();
void updateWorldLoop();

void loadChunk(Chunk::PosVec position);
Block::Type getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const World* currentChunk);