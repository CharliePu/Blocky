#include "World.h"

//Must delete afterward: create Camera.cpp
extern GLCamera camera;

//Must initialize after GL is set up.
World::World() :
	frontDrawBuffer(), backDrawBuffer(), unloadBuffer(),
	updateThread(&World::updateWorldLoop, this)
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

void World::unloadDistantChunks()
{
	if (!unloadBuffer.empty())
	{
		chunkMap.erase(Chunk::PosVec(unloadBuffer.front()->chunkX, unloadBuffer.front()->chunkZ));
		delete unloadBuffer.front();
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

		//load chunks around current position within radius
		Chunk::PosVec currentPosition(std::floor(camera.position.x / Chunk::sizeX), std::floor(camera.position.z / Chunk::sizeZ));
		for (Chunk::Position i = 0; i <= renderSize; i++)
			for (Chunk::Position j = 0; j <= renderSize; j++)
			{
				loadChunk(Chunk::PosVec(currentPosition) - renderSize / 2 + Chunk::PosVec(i, j));
			}

		//swap two buffer of chunks
		bufferLock.lock();
		std::swap(frontDrawBuffer, backDrawBuffer);
		bufferLock.unlock();

		//delete far chunk, update modified chunk
		for (auto i = backDrawBuffer.cbegin(); i != backDrawBuffer.cend();)
		{
			auto j = *i;
			if ((abs((j)->chunkX - currentPosition.x) > renderSize / 2) || (abs((j)->chunkZ - currentPosition.y) > renderSize / 2))
			{
				i = backDrawBuffer.erase(i);
				unloadBuffer.push(j);
			}
			else
			{
				++i;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}
	updateThreadShouldClose = false;
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
		chunkMap[position]->update();
	}

	backDrawBuffer.push_back(chunkMap[position]);
}