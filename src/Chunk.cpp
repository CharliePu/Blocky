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
				y < (glm::simplex(glm::vec2((x + noiseX)/10.0,(z + noiseZ)/10.0)) + 1) * 10;
				++y)
			{
				data[x][y][z] = static_cast<Block::Type>(Block::Type::DIRT);
			}
		}
}

void Chunk::draw()
{
	if (this->needUpdate)
	{
		if (!this->vao)
		{
			glGenVertexArrays(1, &this->vao);
			glGenBuffers(1, &this->vbo);
		}
		if (vao == 0 || vbo == 0)
			sendError("Shit again!");

		glBindVertexArray(this->vao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, verticesBuffer[0].size() * sizeof(Vertex), &verticesBuffer[0][0], GL_STATIC_DRAW);
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

void Chunk::update()
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

void Chunk::save()
{
	std::ofstream chunkFile;
	chunkFile.open("Saves\\chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::out | std::ios::trunc | std::ios::binary);
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

void Chunk::addBlockVertices(const Block::Position &x, const Block::Position &y, const Block::Position &z, std::vector<Vertex> verticesGroups[])
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

//Must initialize after GL is set up.
World::World() :
	frontDrawBuffer(new std::list<Chunk*>), backDrawBuffer(new std::list<Chunk*>), unloadBuffer(),
	updateThread(&World::updateWorldLoop, this)
{
	blockTexture[Block::Type::AIR] = 0;
	blockTexture[Block::Type::DIRT] = Texture("..\\src\\resource\\dirt.bmp", texture_diffuse_map);
	blockTexture[Block::Type::COBBLESTONE] = Texture("..\\src\\resource\\cobblestone.bmp", texture_diffuse_map);
	blockTexture[Block::Type::GRASS] = Texture("..\\src\\resource\\grass.bmp", texture_diffuse_map);
	blockTexture[Block::Type::BEDROCK] = Texture("..\\src\\resource\\bedrock.bmp", texture_diffuse_map);
}
		

void World::draw()
{
	bufferLock.lock();
	if (frontDrawBuffer)
	{
		for (auto i : *frontDrawBuffer)
		{
				i->draw();
		}
	}
	bufferLock.unlock();
}

void World::drawDebug()
{
	bufferLock.lock();
	if (frontDrawBuffer)
	{
		for (auto i : *frontDrawBuffer)
		{
			i->debug();
		}
	}
	bufferLock.unlock();
}

void World::enableUpdateThread()
{
	updateThreadShouldClose = false;
	updateThread.detach();
}

void World::disableUpdateThread()
{
	updateThreadShouldClose = true;
}

void World::updateWorldLoop()
{	
	Chunk::Position currentRenderSize = 0;

	while (!updateThreadShouldClose)
	{
		//clear back draw buffer
		backDrawBuffer->clear();

		//increase load radius gradually
		if (currentRenderSize < World::renderSize)
			currentRenderSize++;

		//load chunks around current position within load radius
		Chunk::PosVec currentPosition(getCameraPosition().x / Chunk::sizeX, getCameraPosition().z / Chunk::sizeZ);
		for (Chunk::Position i = 0; i <= currentRenderSize; i++)
			for (Chunk::Position j = 0; j <= currentRenderSize; j++)
			{
				loadChunk(Chunk::PosVec(currentPosition) - currentRenderSize / 2 + Chunk::PosVec(i, j));
			}

		//delete far chunk, update modified chunk
		for (auto i = backDrawBuffer->begin(); i != backDrawBuffer->end();)
		{
			auto j = *i;
			if ((abs((j)->chunkX - currentPosition.x) > World::renderSize / 2) || (abs((j)->chunkZ - currentPosition.y) > World::renderSize / 2))
			{
				i = backDrawBuffer->erase(i);
				unloadBuffer.push_back(j);
			}
			else if (!j -> needUpdate)
			{
				j->update();
				++i;
			}
		}
		sendError(".");

		//swap two buffer of chunks
		bufferLock.lock();
		std::swap(frontDrawBuffer, backDrawBuffer);
		bufferLock.unlock();

		//unload chunks
		for (auto i : unloadBuffer)
		{
			i->save();
			chunkMap.erase(Chunk::PosVec(i->chunkX, i->chunkZ));
			delete i;
		}
		unloadBuffer.clear();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	for (auto i : chunkMap)
	{
		i.second->save();
		delete i.second;
	}
	delete frontDrawBuffer;
	delete backDrawBuffer;
}

void World::loadChunk(Chunk::PosVec position)
{
	if (chunkMap.find(position) == chunkMap.end())
	{
		chunkMap[position] = new Chunk(position);

		if (!chunkMap[position]->load())
		{
			chunkMap[position]->generate(position.x*Chunk::sizeX, position.y*Chunk::sizeZ);
		}
	}

	backDrawBuffer->push_back(chunkMap[position]);
}