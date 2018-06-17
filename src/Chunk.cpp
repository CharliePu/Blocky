#include "Chunk.h"

GLuint blockTexture[Block::typeNum];

const glm::fvec3 Block::vertexPositions[]
{
	glm::vec3(-0.5, 0.5, 0.5),
	glm::vec3(-0.5, -0.5, 0.5),
	glm::vec3(0.5, -0.5, 0.5),
	glm::vec3(0.5, 0.5, 0.5),
	glm::vec3(-0.5, 0.5, -0.5),
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(0.5, 0.5, -0.5)
};

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

void initBlockRenderer()
{
	//load blocks' textures
	blockTexture[Block::Type::DIRT] = Texture("D:\\Projects\\Blocky - OpenGL\\src\\resource\\dirt.bmp", texture_diffuse_map);
	blockTexture[Block::Type::COBBLESTONE] = Texture("D:\\Projects\\Blocky - OpenGL\\src\\resource\\cobblestone.bmp", texture_diffuse_map);
	blockTexture[Block::Type::GRASS] = Texture("D:\\Projects\\Blocky - OpenGL\\src\\resource\\grass.bmp", texture_diffuse_map);
	blockTexture[Block::Type::BEDROCK] = Texture("D:\\Projects\\Blocky - OpenGL\\src\\resource\\bedrock.bmp", texture_diffuse_map);
}

World::~World()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &debugVao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &debugVbo);
}

void World::generate(globalBlockPos noiseX, globalBlockPos noiseZ)
{
	//(perlinNoise2D((noiseX + x - 1) / 20.0, (noiseZ + z - 1) / 20.0) + 1) * 50
	for (Block::Position x = 0; x !=  Chunk::sizeX + 2; ++x)
		for (Block::Position z = 0; z != Chunk::sizeZ + 2; ++z)
		{
			for (Block::Position y = 0; 
				y != static_cast<Block::Position>(3) && y != Chunk::sizeY;
				++y)
			{
				data[x][y][z] = Block::Type::COBBLESTONE;
			}
		}
}

void World::draw()
{
	if (this->needUpdate)
	{
		if (!this->vao)
		{
			glGenVertexArrays(1, &this->vao);
			glGenBuffers(1, &this->vbo);
		}

		glBindVertexArray(this->vao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, verticesBuffer[Block::Type::AIR].size() * sizeof(Vertex), &verticesBuffer[Block::Type::AIR][0], GL_STATIC_DRAW);
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

		this->needUpdate = false;
	}
	glBindVertexArray(this->vao);
	//air is ignored
	for (auto i = decltype(Block::typeNum)(); i != Block::typeNum; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, blockTexture[i]);
		glDrawArrays(GL_TRIANGLES, verticesOffset[i], verticesOffset[i + 1] - verticesOffset[i]);
	}
	glBindVertexArray(NULL);
}

void World::update()
{
	for (auto i = decltype(Block::typeNum)(); i != Block::typeNum; ++i)
		verticesBuffer[i].clear();

	//add block vertices
	for (Block::Position x = 1; x != Chunk::sizeX + 1; ++x)
		for (Block::Position y = 1; y != Chunk::sizeY + 1; ++y)
			for (Block::Position z = 1; z != Chunk::sizeZ + 1; ++z)
			{
				addBlockVertices(x, y, z, verticesBuffer);
			}

	//combine them (ignore air)
	for (auto i = decltype(Block::typeNum)(); i != Block::typeNum; ++i)
	{
		verticesBuffer[Block::Type::AIR].insert(verticesBuffer[Block::Type::AIR].end(), verticesBuffer[i].begin(), verticesBuffer[i].end());
		verticesOffset[i + 1] = static_cast<GLsizei>(verticesBuffer[Block::Type::AIR].size());
	}

	this->needUpdate = true;
}

void World::save()
{
	std::ofstream chunkFile;
	chunkFile.open("Saves/chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::out | std::ios::trunc | std::ios::binary);
	if (chunkFile.good())
	{
		for (const auto &i : this->data)
			for (const auto &j : i)
				for (const auto &k : j)
				{
					chunkFile.write((char*)&k, sizeof(Block::Type));
				}
	}
	chunkFile.close();
}

bool World::load()
{
	std::ifstream chunkFile;
	chunkFile.open("Saves/chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::in | std::ios::binary);
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

void World::debug()
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

void World::addBlockVertices(const Block::Position &x, const Block::Position &y, const Block::Position &z, std::vector<Vertex> verticesGroups[])
{
	//do nothing if the block is air
	if (data[x][y][z] == Block::Type::AIR)
	{
		return;
	}
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
			verticesGroups[data[x][y][z]].push_back(temp);
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
			verticesGroups[data[x][y][z]].push_back(temp);
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
			verticesGroups[data[x][y][z]].push_back(temp);
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
			verticesGroups[data[x][y][z]].push_back(temp);
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
			verticesGroups[data[x][y][z]].push_back(temp);
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
			verticesGroups[data[x][y][z]].push_back(temp);
		}
	}
}

struct KeyHasher
{
	std::size_t operator()(const Chunk::PosVec& key) const
	{
		using std::size_t;
		using std::hash;

		return ((key.x * 5209) ^ (key.y * 1811));
	}
};

std::unordered_map <Chunk::PosVec, World*, KeyHasher> chunkMap;

std::vector<World*> *frontDrawBuffer;
std::vector<World*>	*backDrawBuffer;


std::mutex updateLock;
std::mutex bufferLock;

void drawWorld()
{
	bufferLock.lock();
	for (auto i : *frontDrawBuffer)
	{
		i->draw();
	}
	bufferLock.unlock();
}

void drawWorldDebug()
{
	bufferLock.lock();
	for (auto i : *frontDrawBuffer)
	{
		i->debug();
	}
	bufferLock.unlock();
}

bool updateThreadShouldClose;
std::thread updateThread;
void enableUpdateThread()
{
	updateThread = std::thread(updateWorldLoop);
	updateThread.detach();
	updateThreadShouldClose = false;
}

void disableUpdateThread()
{
	updateLock.lock();
	updateThreadShouldClose = true;
	updateLock.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void updateWorldLoop()
{
	frontDrawBuffer = new std::vector<World*>;
	backDrawBuffer = new std::vector<World*>;
	
	Chunk::Position currentRenderSize = 0;

	while (!updateThreadShouldClose)
	{
		//clear back draw buffer
		backDrawBuffer->clear();

		//increase load radius gradually
		if (currentRenderSize < renderSize)
			currentRenderSize++;

		//load chunks around current position within load radius
		Chunk::PosVec currentPosition(getCameraPosition().x / Chunk::sizeX, getCameraPosition().z / Chunk::sizeZ);
		for (Chunk::Position i = 0; i <= currentRenderSize; i++)
			for (Chunk::Position j = 0; j <= currentRenderSize; j++)
			{
				loadChunk(Chunk::PosVec(currentPosition) - currentRenderSize / 2 + Chunk::PosVec(i, j));
			}

		updateLock.lock();
		//update chunks that needed
		for (auto i : *backDrawBuffer)
		{
			if (!i->needUpdate)
			{
				i->update();
			}
		}

		bufferLock.lock();
		//swap two buffer of chunks
		std::swap(frontDrawBuffer, backDrawBuffer);
		bufferLock.unlock();

		//unload chunks
		for (auto i : *backDrawBuffer)
		{
			if ((abs(i->chunkX - currentPosition.x) > renderSize/2) || (abs(i->chunkZ - currentPosition.y) > renderSize/2))
			{
				i->save();
				chunkMap.erase(Chunk::PosVec(i->chunkX, i->chunkZ));
				delete i;
			}
		}
		updateLock.unlock();

	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	for (auto i : chunkMap)
	{
		i.second->save();
		delete i.second;
	}
	delete frontDrawBuffer;
	delete backDrawBuffer;
}

void loadChunk(Chunk::PosVec position)
{
	updateLock.lock();
	if (!chunkMap[position])
	{
		chunkMap[position] = new World(position);
		if (!chunkMap[position]->load())
		{
			chunkMap[position]->generate(position.x*Chunk::sizeX, position.y*Chunk::sizeZ);
		}
	}
	backDrawBuffer->push_back(chunkMap[position]);
	updateLock.unlock();
}

Block::Type getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const World* currentChunk)
{
	if (!currentChunk)
	{
		sendError("getBlockType: nullptr receieved");
		return Block::Type::AIR;
	}
	if (y >= Chunk::sizeY || y < 0)
	{
		sendError("getBlockType: abnormal y-value receieved");
		return Block::Type::AIR;
	}
	
	Block::Position localX, localY, localZ;
	Chunk::Position offSetchunkX, offSetChunkZ;

	localY = (Block::Position)y;

	localX = (Block::Position)(x % Chunk::sizeX);
	offSetchunkX = (Chunk::Position)(x / Chunk::sizeX);
	if (x < 0)
	{
		localX += Chunk::sizeX;
		offSetchunkX--;
	}

	localZ = (Block::Position)(z % Chunk::sizeZ);
	offSetChunkZ = (Chunk::Position)(z / Chunk::sizeZ);
	if (z < 0)
	{
		localZ += Chunk::sizeZ;
		offSetChunkZ--;
	}

	if (chunkMap.find(Chunk::PosVec(offSetchunkX + currentChunk->chunkX, offSetChunkZ + currentChunk->chunkZ))==chunkMap.end())
	{
		//sendError("getBlockType: positioned chunk invalid");
		return Block::Type::AIR;
	}

	return chunkMap[Chunk::PosVec(offSetchunkX + currentChunk->chunkX, offSetChunkZ + currentChunk->chunkZ)]->data[localX][localY][localZ];
}

Block::Type getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z)
{
	if (y >= Chunk::sizeY || y < 0)
	{
		sendError("getBlockType: abnormal y-value receieved");
		return Block::Type::AIR;
	}

	Block::Position localX, localY, localZ;
	Chunk::Position localChunkX, localChunkZ;

	localY = (Block::Position)y;

	localX = (Block::Position)(x % Chunk::sizeX);
	localChunkX = (Chunk::Position)(x / Chunk::sizeX);
	if (x < 0)
	{
		localX += Chunk::sizeX;
		localChunkX--;
	}

	localZ = (Block::Position)(z % Chunk::sizeZ);
	localChunkZ = (Chunk::Position)(z / Chunk::sizeZ);
	if (z < 0)
	{
		localZ += Chunk::sizeZ;
		localChunkZ--;
	}

	if (chunkMap.find(Chunk::PosVec(localChunkX, localChunkZ)) == chunkMap.end())
	{
		//sendError("getBlockType: positioned chunk invalid");
		return Block::Type::AIR;
	}

	return chunkMap[Chunk::PosVec(localChunkX, localChunkZ)]->data[localX][localY][localZ];
}
