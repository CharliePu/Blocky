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

	static Block::PosVec toLocalPosition(const Block::GlobalPosVec &pos);
	static Chunk::PosVec toChunkPosition(const Block::GlobalPosVec &pos);

private:
	GLuint vao, vbo, debugVao, debugVbo;
	std::vector<Vertex> vertexBuffer;
	std::array<GLsizei, Block::typeNum + 1> verticesOffset;

	void addBlockVertices(const Block::Position & x, const Block::Position & y, const Block::Position & z, std::vector<Vertex> &verticesGroups);
};

inline Block::PosVec Chunk::toLocalPosition(const Block::GlobalPosVec &pos)
{
	auto lx = pos.x % Chunk::sizeX;
	if (lx < 0) lx += Chunk::sizeX;

	auto ly = pos.y;

	auto lz = pos.z % Chunk::sizeZ;
	if (lz < 0) lz += Chunk::sizeZ;

	return Block::PosVec(lx, ly, lz);
}

inline Chunk::PosVec Chunk::toChunkPosition(const Block::GlobalPosVec & pos)
{
	return Chunk::PosVec(static_cast<Chunk::Position>(std::floor(pos.x / static_cast<float>(Chunk::sizeX))),
		static_cast<Chunk::Position>(std::floor(pos.z / static_cast<float>(Chunk::sizeZ))));
}