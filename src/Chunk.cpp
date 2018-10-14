#include "Chunk.h"
#include "include/Noise.h"

Chunk::Chunk(Chunk::PosVec position) :
	chunkX(position.x), chunkZ(position.y),
	needBindBuffer(true), 
	vao(0), vbo(0),
	debugVao(0), debugVbo(0),
	vertexBuffer(), verticesOffset(),
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
	static std::default_random_engine e;
	static std::uniform_int_distribution<unsigned> u(10, 20);
	static std::bernoulli_distribution treeShouldBeHere(0.005);

	for (Block::Position x = 0; x != Chunk::sizeX + bufferLength * 2; ++x)
		for (Block::Position z = 0; z != Chunk::sizeZ + bufferLength * 2; ++z)
		{
			Block::Position h = static_cast<int>((perlinNoise2D((noiseX + x) / 200.0f, (noiseZ + z) / 200.0f) + 1) * 200);
			for (Block::Position y = 0; y != h; ++y)
			{
				if (y < 150)
					data[x][y][z] = Block::Type::SAND;
				else
					data[x][y][z] = Block::Type::DIRT;
			}
			if (h < 147)
			{
				data[x][h++][z] = Block::Type::SAND;
				for (Block::Position y = h; y < 144; ++y)
					data[x][y][z] = Block::Type::WATER;
			}
			else
			{
				data[x][h][z] = Block::Type::GRASS;
			}
			
			if (h > 150)
			{
				if (x > bufferLength - 1 + 2 && x < Chunk::sizeX + bufferLength - 3
					&& z > bufferLength - 1 + 2 && z < Chunk::sizeZ + bufferLength - 3
					&& treeShouldBeHere(e))
				{
					data[x][h + 1][z] = Block::Type::BARK;
					data[x][h + 2][z] = Block::Type::BARK;
					data[x][h + 3][z] = Block::Type::BARK;
					data[x][h + 4][z] = Block::Type::BARK;
					data[x][h + 5][z] = Block::Type::BARK;
					data[x][h + 6][z] = Block::Type::BARK;

					for (auto i : Tree::positions)
						data[x + i.x][h + 5 + i.y][z + i.z] = Block::Type::LEAVES;
				}
			}
		}
}

void Chunk::draw()
{
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
		for (int i = 1; i != static_cast<int>(Block::Type::COUNT); ++i)
			if (i != static_cast<int>(Block::Type::WATER))
			{
				Texture::get(static_cast<Block::Type>(i)).use();
				glDrawArrays(GL_TRIANGLES, verticesOffset[i], verticesOffset[i + 1] - verticesOffset[i]);
			}

		//draw transparent at last
		Texture::get(Block::Type::WATER).use();
		glDrawArrays(GL_TRIANGLES, verticesOffset[static_cast<size_t>(Block::Type::WATER)], verticesOffset[static_cast<size_t>(Block::Type::WATER) + 1] - verticesOffset[static_cast<size_t>(Block::Type::WATER)]);
	glBindVertexArray(NULL);
}

void Chunk::update()
{
	//Temporary vertex buffer
	std::vector<Vertex> tempVertexBuffer[static_cast<size_t>(Block::Type::COUNT)]{};

	vertexBuffer.clear();

	//std::cout << "go";
	//add block vertices
	for (Block::Position x = bufferLength; x != Chunk::sizeX + bufferLength; ++x)
		for (Block::Position y = bufferLength; y != Chunk::sizeY + bufferLength; ++y)
			for (Block::Position z = bufferLength; z != Chunk::sizeZ + bufferLength; ++z)
				if (Block::isVisibleBlock(data[x][y][z]))
				{
					addBlockVertices(x, y, z, tempVertexBuffer[static_cast<size_t>(data[x][y][z])]);
				}

	//combine vertices, ignore air
	for (Block::TypeInt i = 0u; i != static_cast<Block::TypeInt>(Block::Type::COUNT); ++i)
	{
		vertexBuffer.insert(vertexBuffer.end(), tempVertexBuffer[i].begin(), tempVertexBuffer[i].end());
		verticesOffset[i + 1] = static_cast<GLsizei>(vertexBuffer.size());

	}
	needBindBuffer = true;
}

void Chunk::save()
{
	std::ofstream chunkFile;
	chunkFile.open("saves\\chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::out | std::ios::trunc | std::ios::binary);
	if (chunkFile.good())
	{
		for (const auto &i : data)
			for (const auto &j : i)
				for (const auto &k : j)
				{
					try
					{
						chunkFile.write((char*)&k, sizeof(Block::Type));
					}
					catch (const std::exception &e)
					{
						std::cerr << "Failed to write chunk" << chunkX << ", " << chunkZ << ": " << e.what() << std::endl;
						break;
					}
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
			{
				try
				{
					chunkFile.read((char*)&k, sizeof(Block::Type));
				}
				catch (const std::exception &e)
				{
					std::cerr << "Failed to read chunk" << chunkX << ", " << chunkZ << ": " << e.what() << std::endl;
					chunkFile.close();
					return false;
				}
			}

	chunkFile.close();
	return true;
}

void Chunk::debug()
{
	if (!this->debugVao)
	{
		GLfloat debugFramework[] =
		{
			(this->chunkX + 1) * Chunk::sizeX,0.0f			,this->chunkZ * Chunk::sizeZ		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,

			this->chunkX * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,
			this->chunkX * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			this->chunkX * Chunk::sizeX,0.0f			,this->chunkZ * Chunk::sizeZ		,1,1,1,
			this->chunkX * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,

			this->chunkX * Chunk::sizeX,0.0f			,this->chunkZ * Chunk::sizeZ		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,0.0f			,this->chunkZ * Chunk::sizeZ		,1,1,1,

			this->chunkX * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			this->chunkX * Chunk::sizeX,0.0f			,this->chunkZ * Chunk::sizeZ		,1,1,1,
			this->chunkX * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX,0.0f		,this->chunkZ * Chunk::sizeZ		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,0.0f			,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			this->chunkX * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,

			this->chunkX * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			this->chunkX * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,
			this->chunkX * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1,

			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,this->chunkZ * Chunk::sizeZ		,1,1,1,
			(this->chunkX + 1) * Chunk::sizeX,Chunk::sizeY	,(this->chunkZ + 1) * Chunk::sizeZ,1,1,1
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
	//each face in anti-clockwise order

	Vertex temp;
	const Block::Type& type(data[x][y][z]), 
		right(data[x + 1][y][z]), 
		left(data[x - 1][y][z]), 
		up(data[x][y + 1][z]), 
		down(data[x][y - 1][z]),
		front(data[x][y][z + 1]),
		back(data[x][y][z - 1]);
	
	//right
	if (right == Block::Type::AIR || right == Block::Type::WATER && type != right)
	{
		temp.normal = Block::vertexNormals[0];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[0][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//left
	if (left == Block::Type::AIR || left == Block::Type::WATER && type != left)
	{
		temp.normal = Block::vertexNormals[1];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[1][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//up
	if (up == Block::Type::AIR || up == Block::Type::WATER && type != up)
	{
		temp.normal = Block::vertexNormals[2];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[2][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//down
	if (down == Block::Type::AIR || down == Block::Type::WATER && type != down)
	{
		temp.normal = Block::vertexNormals[3];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[3][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//front
	if (front == Block::Type::AIR || front == Block::Type::WATER && type != front)
	{
		temp.normal = Block::vertexNormals[4];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[4][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
	//back
	if (back == Block::Type::AIR || back == Block::Type::WATER && type != back)
	{
		temp.normal = Block::vertexNormals[5];
		for (auto i = 0; i < 6; i++)
		{
			temp.position = Block::vertexPositions[Block::vertexIndices[5][i]] + glm::vec3(x + chunkX * Chunk::sizeX, y, z + chunkZ * Chunk::sizeZ) + glm::vec3(-1);
			temp.texCoord = Block::vertexTexCoords[i];
			verticesGroups.push_back(temp);
		}
	}
}