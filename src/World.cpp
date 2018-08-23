#include "World.h"
#include "Player.h"

//Must delete afterward: create Camera.cpp
extern Player camera;


//Must initialize after GL is set up.
World::World() :
	frontDrawBuffer(), backDrawBuffer(), unloadBuffer(),
	frontBufferLock(), unloadBufferLock(), chunkMapLock(), 
	updateThread(&World::updateWorldLoop, this),
	chunkMap(), currentChunkPosition(), currentChunk(),
	currentRenderSize()
{
	static bool isTextureInited = false;
	if (!isTextureInited)
	{
		isTextureInited = true;
	}
}

void World::removeAll()
{
	for (auto i : *chunkMap)
	{
		delete i.second;
	}
}


void World::draw()
{
	frontBufferLock.lock();
	for (auto i : frontDrawBuffer)
	{
		if (chunkInViewFrustrum(i))
			i->draw();
	}
	frontBufferLock.unlock();
}

void World::drawDebug()
{
	frontBufferLock.lock();
	for (auto i : frontDrawBuffer)
	{
		i->debug();
	}
	frontBufferLock.unlock();
}

void World::updateCurrentChunkPosition()
{
	currentChunkPosition.set(glm::ivec2(std::floor((camera.getPosition().x) / Chunk::sizeX), std::floor((camera.getPosition().z) / Chunk::sizeZ)));
}

void World::unloadDistantChunks()
{
	//Delete a chunk per frame
	unloadBufferLock.lock();
	while (!unloadBuffer.empty())
	{
		delete unloadBuffer.front();
		unloadBuffer.pop_front();
	}
	unloadBufferLock.unlock();
}

void World::updateProceduralFog()
{
	static constexpr float loadingSpeed = 0.1f;
	static float deltaTime, pastTime = 0;
	deltaTime = static_cast<float>(glfwGetTime()) - pastTime;
	static float fogNear = 0, fogFar = Chunk::sizeX;
	if (fogNear < (currentRenderSize - 1) * Chunk::sizeX)
		fogNear += deltaTime * loadingSpeed;
	if (fogFar < currentRenderSize * Chunk::sizeX)
		fogFar += deltaTime * loadingSpeed;
	glUniform1f(getUniformLocation("fogNear"), fogNear);
	glUniform1f(getUniformLocation("fogFar"), fogFar);
}

void World::enableUpdateThread()
{
	updateThreadShouldClose = false;
	updateThread.detach();
}

void World::disableUpdateThread()
{
	updateThreadShouldClose = true;
	//Wait until thread finished
	while (updateThreadShouldClose);
}

void World::updateWorldLoop()
{
	// first generation
	Chunk::PosVec centerChunkPosition = currentChunkPosition.get();
	loadChunk(centerChunkPosition);
	for (int i = 1; i <= renderSize; ++i)
	{
		for (int x = -i; x != i; ++x)
			loadChunk(centerChunkPosition + Chunk::PosVec(x, i));

		for (int z = i; z != -i; --z)
			loadChunk(centerChunkPosition + Chunk::PosVec(i, z));

		for (int x = i; x != -i; --x)
			loadChunk(centerChunkPosition + Chunk::PosVec(x, -i));

		for (int z = -i; z != i; ++z)
			loadChunk(centerChunkPosition + Chunk::PosVec(-i, z));

		frontBufferLock.lock();
		frontDrawBuffer = backDrawBuffer;
		frontBufferLock.unlock();

		currentRenderSize = i;
		if (updateThreadShouldClose)
		{
			updateThreadShouldClose = false;
			return;
		}
	}

	while (!updateThreadShouldClose)
	{
		centerChunkPosition = currentChunkPosition.get();

		//clear back draw buffer
		backDrawBuffer.clear();

		//load chunks around current position within radius
		for (auto i = -renderSize; i <= renderSize; i++)
			for (auto j = -renderSize; j <= renderSize; j++)
			{
				loadChunk(centerChunkPosition + Chunk::PosVec(i, j));
			}

		//swap two buffer of chunks
		frontBufferLock.lock();
		frontDrawBuffer.swap(backDrawBuffer);
		frontBufferLock.unlock();

		unloadBufferLock.lock();
		//put far chunks into unload buffer
		for (auto i : backDrawBuffer)
		{
			if (chunkOutsideRenderZone(*i, centerChunkPosition, renderSize))
			{
				unloadBuffer.push_back(i);
				//erase from chunkMap so renderer has no way to access it again before it is deleted
				chunkMap.operateWriting([&]
				{
					chunkMap->erase(Chunk::PosVec(i->chunkX, i->chunkZ));
				});
				//reset to nullptr if currentChunk is going to be deleted
				if (i == currentChunk)
				{
					currentChunk = nullptr;
				}
			}
		}
		unloadBufferLock.unlock();

		//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	updateThreadShouldClose = false;
}

void World::loadChunk(Chunk::PosVec position)
{
	Chunk *chunk;
	if (chunkMap.operateReading<bool>([&] { return chunkMap->find(position) == chunkMap->cend(); }))
	{
		//allocate new Chunk
		try
		{
			chunk = new Chunk(position);
		}
		catch (std::exception e)
		{
			std::cerr << e.what() << std::endl;
		}

		//prevent reading in other threads while STL container is being written
		//chunkMapLock.lock();
		chunkMap.operateWriting([&]{
			chunkMap->insert({ position, chunk });
			});
		//chunkMapLock.unlock();

		if (!chunk->load())
		{
			chunk->generate(position.x*Chunk::sizeX, position.y*Chunk::sizeZ);
		}
		chunk->update();
	}
	else
	{
		chunk = chunkMap.operateReading<Chunk*>([&] { return (*chunkMap)[position]; });
	}
	backDrawBuffer.push_back(chunk);
}

Chunk * const World::getCurrentChunk()
{
	//thread-safe reading
	Chunk::PosVec currentChunkPosition(this->currentChunkPosition.get());

	//if currentchunk exists and equals to chunk position, use old value
	if (currentChunk)
		if (currentChunk->chunkX == currentChunkPosition.x && currentChunk->chunkZ == currentChunkPosition.y)
			return currentChunk;

	//Find chunk in chunkmap, if no found, use old value
	decltype(chunkMap->cend()) findCurrentChunk, chunkEnd;
	chunkMap.operateReading([&] {
		findCurrentChunk = chunkMap->find(currentChunkPosition);
		chunkEnd = chunkMap->cend();
	});

	currentChunk = findCurrentChunk == chunkEnd ? currentChunk : (*findCurrentChunk).second;

	return currentChunk;
}

Block::Type * World::findBlock(const Block::GlobalPosVec pos)
{
	//return nullptr if y outside chunk boundry
	if (pos.y < 0 || pos.y >= Chunk::sizeY) return nullptr;

	auto localPos = Chunk::toLocalPosition(pos);
	auto chunkPos = Chunk::toChunkPosition(pos);

	decltype(chunkMap->cend()) chunkMapFind, chunkEnd;
	chunkMap.operateReading([&] {
		chunkMapFind = chunkMap->find(chunkPos);
		chunkEnd = chunkMap->cend();
	});

	////Find target chunk
	//chunkMapLock.lock();
	//auto chunkMapFind = chunkMap.find(chunkPos);
	//auto chunkEnd = chunkMap.cend();
	//chunkMapLock.unlock();

	if (chunkMapFind != chunkEnd)
	{
		//lx + 1, lz + 1 for padding
		return &chunkMapFind->second->data[localPos.x + 1][localPos.y][localPos.z + 1];
	}
	else
	{
		//return nullptr if chunk not found
		return nullptr;
	}
}

Block::Type World::getBlock(const Block::GlobalPosVec &pos)
{
	Block::Type *block = findBlock(pos);
	if (block)
		return *block;
	else
		return Block::AIR;
}

//return success or not
bool World::setBlock(const Block::GlobalPosVec &pos, const Block::Type &type)
{
	Block::Type *block = findBlock(pos);
	if (!block)
		return false;
	*block = type;

	updateChunksForBlock(pos);
	return true;
}

void World::updateChunksForBlock(const Block::GlobalPosVec & pos)
{
	auto localPos = Chunk::toLocalPosition(pos);
	decltype(chunkMap->end()) findChunk;
	chunkMap.operateReading([&]
	{
		findChunk = chunkMap->find(Chunk::toChunkPosition(pos));
		if (findChunk == chunkMap->end())
		{
			std::cerr << "UpdateChunkForBlock(GlobalPosVec): Chunk no exist" << std::endl;
			return;
		}
	});

	Chunk * const &chunk = findChunk->second;
	chunk->update();
	chunkMap.operateReading([&] 
	{
		auto chunkNotFound = chunkMap->end();
		auto &blockType = chunk->data[localPos.x + 1][localPos.y][localPos.z + 1];
		if (localPos.x == 0)
		{
			auto aChunk = chunkMap->find({ chunk->chunkX - 1, chunk->chunkZ });
			if (aChunk != chunkNotFound)
			{
				aChunk->second->data[Chunk::sizeX + 1][localPos.y][localPos.z + 1] = blockType;
				aChunk->second->update();
			}
		}
		if (localPos.x == Chunk::sizeX - 1)
		{
			auto aChunk = chunkMap->find({ chunk->chunkX + 1, chunk->chunkZ });
			if (aChunk != chunkNotFound)
			{
				aChunk->second->data[0][localPos.y][localPos.z + 1] = blockType;
				aChunk->second->update();
			}
		}
		if (localPos.z == 0)
		{
			auto aChunk = chunkMap->find({ chunk->chunkX, chunk->chunkZ - 1 });
			if (aChunk != chunkNotFound)
			{
				aChunk->second->data[localPos.x + 1][localPos.y][Chunk::sizeX + 1] = blockType;
				aChunk->second->update();
			}
		}
		if (localPos.z == Chunk::sizeZ - 1)
		{
			auto aChunk = chunkMap->find({ chunk->chunkX, chunk->chunkZ + 1 });
			if (aChunk != chunkNotFound)
			{
				aChunk->second->data[localPos.x + 1][localPos.y][0] = blockType;
				aChunk->second->update();
			}
		}
	}
	);
}

//cull chunks before rendering
bool World::chunkInViewFrustrum(Chunk * const & chunk)
{
	static const float cutOff = glm::radians(90.0f);

	if (std::abs(chunk->chunkX - currentChunkPosition->x) <= 1 &&
		std::abs(chunk->chunkZ - currentChunkPosition->y) <= 1)
		return true;

	glm::vec3 chunkPos((chunk->chunkX + 0.5) * Chunk::sizeX, Chunk::sizeY / 2,(chunk->chunkZ + 0.5) * Chunk::sizeX);
	glm::vec3 chunkDir = glm::normalize(chunkPos - camera.getPosition());
	glm::vec3 cameraDir = glm::normalize(glm::vec3(-camera.getFront().x, camera.getFront().y, -camera.getFront().z));
	float angle = std::acos(glm::dot(chunkDir, cameraDir));
	if (angle < cutOff)
		return true;
	else
	{
		chunkPos = glm::vec3((chunk->chunkX + 0.5) * Chunk::sizeX, camera.getPosition().y, (chunk->chunkZ + 0.5) * Chunk::sizeX);
		chunkDir = glm::normalize(chunkPos - camera.getPosition());
		if (std::acos(glm::dot(chunkDir, cameraDir)) < cutOff)
			return true;
		else
		{
			return false;
		}
	}
}
