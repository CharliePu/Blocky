#include "Chunk.h"

Chunk::Chunk(Chunk::PosVec position) :
	chunkX(position.x), chunkZ(position.y),
	needBindBuffer(true), needUpdate(true),
	vao(0), vbo(0),
	debugVao(0), debugVbo(0),
	data() {};

//must be called under main thread
Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &debugVao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &debugVbo);
}

void Chunk::generate(Block::GlobalPosition noiseX, Block::GlobalPosition noiseZ)
{
	//(perlinNoise2D((noiseX + x - 1) / 20.0, (noiseZ + z - 1) / 20.0) + 1) * 50
	for (Block::Position x = 0; x !=  Chunk::sizeX + 2; ++x)
		for (Block::Position z = 0; z != Chunk::sizeZ + 2; ++z)
		{
			
			for (Block::Position y = 0; 
				y < (glm::simplex(glm::vec2((x + noiseX)/50.0,(z + noiseZ)/50.0)) + 1) * 20;
				++y)
			{
				data[x][y][z] = static_cast<Block::Type>(Block::Type::COBBLESTONE);
			}
		}
}

void Chunk::draw()
{
	if (this->needUpdate)
		return;
	if (this->needBindBuffer)
	{
		if (!this->vao)
		{
			glGenVertexArrays(1, &this->vao);
			glGenBuffers(1, &this->vbo);
		}

		glBindVertexArray(this->vao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);
		//set position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
		//set normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		//set texCoords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
		glBindVertexArray(NULL);

		this->needBindBuffer = false;
	}
	glBindVertexArray(this->vao);
	//air is ignored
	for (auto i = decltype(Block::typeNum)(); i != Block::typeNum; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, Texture::get(i));
		glDrawArrays(GL_TRIANGLES, verticesOffset[i], verticesOffset[i + 1] - verticesOffset[i]);
	}
	glBindVertexArray(NULL);
}

void Chunk::update()
{
	if (!needUpdate)
	{
		return;
	}
	//Temporary vertex buffer
	std::vector<Vertex> tempVertexBuffer[Block::typeNum]{};

	vertexBuffer.clear();

	//add block vertices
	for (Block::Position x = 1; x != Chunk::sizeX + 1; ++x)
		for (Block::Position y = 1; y != Chunk::sizeY + 1; ++y)
			for (Block::Position z = 1; z != Chunk::sizeZ + 1; ++z)
				if (data[x][y][z])
				{
					addBlockVertices(x, y, z, tempVertexBuffer[data[x][y][z]]);
				}

	//combine vertices, ignore air
 	for (auto i = decltype(Block::typeNum)(1); i != Block::typeNum; ++i)
	{
		vertexBuffer.insert(vertexBuffer.end(), tempVertexBuffer[i].begin(), tempVertexBuffer[i].end());
		verticesOffset[i] = static_cast<GLsizei>(vertexBuffer.size());
	}

	needUpdate = false;
}

void Chunk::save()
{
	std::ofstream chunkFile;
	chunkFile.open("Saves\\chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::out | std::ios::trunc | std::ios::binary);
	if (chunkFile.good())
	{
		for (const auto &i : data)
			for (const auto &j : i)
				for (const auto &k : j)
				{
					chunkFile.write((char*)&k, sizeof(Block::Type));
				}
	}
	else
	{
		std::cerr << "Unable to open Saves\\chunk" + std::to_string(chunkX) + ", " + std::to_string(chunkZ) + ".dat";
	}
	chunkFile.close();
}

bool Chunk::load()
{
	std::ifstream chunkFile;
	chunkFile.open("Saves\\chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::in | std::ios::binary);
	if (!chunkFile.good())
	{
		chunkFile.close();
		return false;
	}

	for (auto &i : this->data)
		for (auto &j : i)
			for (auto &k : j)
				chunkFile.read((char*)&k, sizeof(Block::Type));

	chunkFile.close();
	return true;
}

void Chunk::debug()
{
	if (!this->debugVao)
	{
		GLfloat debugFramework[] =
		{
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f			,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,
			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f			,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			this->chunkX * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f		,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			this->chunkX * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,this->chunkZ * Chunk::sizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX - 0.5f,Chunk::sizeY - 0.5f	,(this->chunkZ + 1) * Chunk::sizeZ - 0.5f,1,1,1
		};
		glGenVertexArrays(1, &this->debugVao);
		glGenBuffers(1, &this->debugVbo);
		glBindVertexArray(this->debugVao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->debugVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(debugFramework), debugFramework, GL_STATIC_DRAW);
		//set position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
		//set color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));
		glBindVertexArray(NULL);
	}
	glBindVertexArray(this->debugVao);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(NULL);
}

void Chunk::addBlockVertices(const Block::Position &x, const Block::Position &y, const Block::Position &z, std::vector<Vertex> &verticesGroups)
{
	//cull adjacent faces
	//each face ordered in anti-clockwise

	Vertex temp;

	//right
	if (data[x + 1][y][z] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[0];
		for (auto i = 0; i < 6; i++)
		{

			temp.texCoord = Block::vertexTexCoords[i];
			temp.position = Block::vertexPositions[Block::vertexIndices[0][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			verticesGroups.push_back(temp);
		}
	}
	//left
	if (data[x - 1][y][z] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[1];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[1][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//up
	if (data[x][y + 1][z] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[2];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[2][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//down
	if (data[x][y - 1][z] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[3];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[3][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//front
	if (data[x][y][z + 1] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[4];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[4][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//back
	if (data[x][y][z - 1] == Block::Type::AIR)
	{
		temp.normal = Block::vertexNormals[5];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[5][i]] + glm::vec3(x + chunkX*Chunk::sizeX, y, z + chunkZ*Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
}