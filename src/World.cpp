#include "World.h"

//Must delete afterward: create Camera.cpp
extern Camera camera;

//Must initialize after GL is set up.
World::World() :
	renderFinished(false),
	frontDrawBuffer(), backDrawBuffer(), unloadBuffer(),
	updateThread(&World::updateWorldLoop, this),
	chunkMap(), currentChunkPosition()
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
	bufferLock.lock();
	for (auto i : frontDrawBuffer)
	{
		i->draw();
	}
	bufferLock.unlock();
}

void World::drawDebug()
{
	bufferLock.lock();
	for (auto i : frontDrawBuffer)
	{
		i->debug();
	}
	bufferLock.unlock();
}

void World::updateCurrentChunkPosition()
{
	currentChunkPosition = glm::ivec2(std::floor(camera.position.x / Chunk::sizeX), std::floor(camera.position.z / Chunk::sizeZ));
}

void World::unloadDistantChunks()
{
	if (!unloadBuffer.empty())
	{
		if (chunkOutsideRenderZone(*unloadBuffer.front()))
		{
			chunkMap.erase(Chunk::PosVec(unloadBuffer.front()->chunkX, unloadBuffer.front()->chunkZ));
			delete unloadBuffer.front();
		}
		unloadBuffer.pop();
	}
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
		//clear back draw buffer
		backDrawBuffer.clear();

		auto centerChunkPosition = this->currentChunkPosition;

		//load chunks around current position within radius
		for (Chunk::Position i = -renderSize; i <= renderSize; i++)
			for (Chunk::Position j = -renderSize; j <= renderSize; j++)
			{
				loadChunk(centerChunkPosition + Chunk::PosVec(i, j));
			}

		//swap two buffer of chunks
		bufferLock.lock();
		std::swap(frontDrawBuffer, backDrawBuffer);
		bufferLock.unlock();

		//delete far chunk, update modified chunk
		for (auto i = backDrawBuffer.cbegin(); i != backDrawBuffer.cend();)
		{
			auto j = *i;
			if (chunkOutsideRenderZone(*j, centerChunkPosition))
			{
				i = backDrawBuffer.erase(i);
				unloadBuffer.push(j);
			}
			else
			{
				++i;
			}
		}

		//mark the first rendering
		if (!renderFinished)
		{
			renderFinished = true;
		}
		//		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	updateThreadShouldClose = false;
}

void World::loadChunk(Chunk::PosVec position)
{
	if (chunkMap.find(position) == chunkMap.end())
	{
		try
		{
			chunkMap.insert({ position, new Chunk(position) });
		}
		catch (const std::exception &e)
		{
			std::cerr << "Failed to create chunk" << position.x << ", " << position.y << " : " << e.what();
		}

		if (!chunkMap[position]->load())
		{
			chunkMap[position]->generate(position.x*Chunk::sizeX, position.y*Chunk::sizeZ);
		}
		chunkMap[position]->update();
	}
	else
	{
		//std::cerr << "passed " << position.x << "," << position.y << std::endl;
	}

	backDrawBuffer.push_back(chunkMap[position]);
}