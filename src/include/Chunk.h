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

class Chunk
{
public:
	static constexpr Block::Position sizeX = 32;
	static constexpr Block::Position sizeY = 3128;
	static constexpr Block::Position sizeZ = 32;

	typedef int Position;
	typedef glm::ivec2 PosVec;

	Chunk(Chunk::PosVec position) :
		chunkX(position.x), chunkZ(position.y),
		needUpdate(false),
		vao(0), vbo(0),
		debugVao(0), debugVbo(0),
		data() {};

	~Chunk();


	void generate(Block::GlobalPosition noiseX, Block::GlobalPosition noiseZ);
	void draw();
	void update();
	void save();
	bool load();
	void debug();
	Chunk::Position chunkX, chunkZ;

	bool needUpdate;
	Block::Type data[Chunk::sizeX + 2][Chunk::sizeY + 2][Chunk::sizeZ + 2];
private:

	GLuint vao, vbo, debugVao, debugVbo;
	std::vector<Vertex> verticesBuffer[Block::typeNum];
	GLsizei verticesOffset[Block::typeNum + 1];

	void addBlockVertices(const Block::Position & x, const Block::Position & y, const Block::Position & z, std::vector<Vertex> verticesGroups[]);
};

class World
{
public:
	static constexpr Chunk::Position renderSize = 2;
	
	World();

	//render chunks
	void draw();
	void drawDebug();

	void enableUpdateThread();
	void disableUpdateThread();

	void updateWorldLoop();
private:
	std::list<Chunk*> *frontDrawBuffer;
	std::list<Chunk*> *backDrawBuffer;
	std::list<Chunk*> unloadBuffer;

	struct KeyHasher
	{
		std::size_t operator()(const Chunk::PosVec& key) const
		{
			return ((key.x * 5209) ^ (key.y * 1811));
		}
	};

	std::unordered_map <Chunk::PosVec, Chunk*, KeyHasher> chunkMap;
	std::mutex bufferLock;

	std::thread updateThread;
	bool updateThreadShouldClose;

	void loadChunk(Chunk::PosVec position);
};