#include "World.h"

//Must delete afterward: create Camera.cpp
extern Player camera;


//Must initialize after GL is set up.
World::World() :
	frontDrawBuffer(), backDrawBuffer(), unloadBuffer(),
	frontBufferLock(), unloadBufferLock(), chunkMapLock(), 
	updateThread(&World::updateWorldLoop, this),
	chunkMap(), currentChunkPosition(), currentChunk()
{
	static bool isTextureInited = false;
	if (!isTextureInited)
	{
		isTextureInited = true;
	}
}

void World::removeAll()
{
	for (auto i : chunkMap)
	{
		delete i.second;
	}
}


void World::draw()
{
	frontBufferLock.lock();
	for (auto i : frontDrawBuffer)
	{
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
	while (!updateThreadShouldClose)
	{
		Chunk::PosVec centerChunkPosition = currentChunkPosition.get();

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
				chunkMap.erase(Chunk::PosVec(i->chunkX, i->chunkZ));
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
	if (chunkMap.find(position) == chunkMap.cend())
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
		chunkMapLock.lock();
		chunkMap.insert({ position, chunk });
		chunkMapLock.unlock();

		if (!chunk->load())
		{
			chunk->generate(position.x*Chunk::sizeX, position.y*Chunk::sizeZ);
		}
		chunk->update();
	}
	else
	{
		chunk = chunkMap[position];
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
	if (chunkMapLock.try_lock())
	{
		auto findCurrentChunk = chunkMap.find(currentChunkPosition);
		currentChunk = findCurrentChunk == chunkMap.cend() ? currentChunk : (*findCurrentChunk).second;
		chunkMapLock.unlock();
	}
	return currentChunk;
}


Block::Type World::getBlock(const Block::GlobalPosition & x, const Block::GlobalPosition & y, const Block::GlobalPosition & z)
{
	//If y outside chunk boundry, return air
	if (y < 0 || y >= Chunk::sizeY) return Block::AIR;
	
	//Convert position into local position
	auto lx = x % Chunk::sizeX;
	if (lx < 0) lx += Chunk::sizeX;
	auto ly = y;
	auto lz = z % Chunk::sizeZ;
	if (lz < 0) lz += Chunk::sizeZ;

	//Find target chunk
	chunkMapLock.lock();
	auto chunkMapFind = chunkMap.find(
		Chunk::PosVec(static_cast<Chunk::Position>(std::floor(x / static_cast<float>(Chunk::sizeX))),
			static_cast<Chunk::Position>(std::floor(z / static_cast<float>(Chunk::sizeZ))))
	);
	auto chunkNotFound = chunkMap.cend();
	chunkMapLock.unlock();

	if (chunkMapFind != chunkNotFound)
	{
		//lx + 1, lz + 1 for padding
		return chunkMapFind->second->data[lx + 1][ly][lz + 1];
	}
	else
	{
		//If chunk not loaded, throw exception
		throw std::exception("getBlock(x, y, z): chunk not found");
	}
}
