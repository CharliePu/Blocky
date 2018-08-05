#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <array>

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/noise.hpp>

#include "Block.h"
#include "Texture.h"

class Chunk
{
public:
	static constexpr Block::Position sizeX = 32;
	static constexpr Block::Position sizeY = 512;
	static constexpr Block::Position sizeZ = 32;

	typedef int Position;
	typedef glm::ivec2 PosVec;

	Chunk(Chunk::PosVec position);
	~Chunk();


	void generate(Block::GlobalPosition noiseX, Block::GlobalPosition noiseZ);
	void draw();
	void update();
	void save();
	bool load();
	void debug();

	Chunk::Position chunkX, chunkZ;

	Block::Type data[Chunk::sizeX + 2][Chunk::sizeY + 2][Chunk::sizeZ + 2];
	bool needBindBuffer, needUpdate;
private:
	GLuint vao, vbo, debugVao, debugVbo;
	std::vector<Vertex> vertexBuffer;
	std::array<GLsizei, Block::typeNum + 1> verticesOffset;

	void addBlockVertices(const Block::Position & x, const Block::Position & y, const Block::Position & z, std::vector<Vertex> &verticesGroups);
};
