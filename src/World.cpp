#include "World.h"

//Must delete afterward: create Camera.cpp
extern GLCamera camera;

//Must initialize after GL is set up.
World::World() :
	frontDrawBuffer(new std::list<Chunk*>), backDrawBuffer(new std::list<Chunk*>), unloadBuffer(),
	updateThread(&World::updateWorldLoop, this)
{
	static bool isTextureInited = false;
	if (!isTextureInited)
	{
		isTextureInited = true;
	}
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
		Chunk::PosVec currentPosition(camera.position.x / Chunk::sizeX, camera.position.z / Chunk::sizeZ);
		for (Chunk::Position i = 0; i <= currentRenderSize; i++)
			for (Chunk::Position j = 0; j <= currentRenderSize; j++)
			{
				loadChunk(Chunk::PosVec(currentPosition) - currentRenderSize / 2 + Chunk::PosVec(i, j));
			}

		//delete far chunk, update modified chunk
		for (auto i = backDrawBuffer->cbegin(); i != backDrawBuffer->cend();)
		{
			auto j = *i;
			if ((abs((j)->chunkX - currentPosition.x) > World::renderSize / 2) || (abs((j)->chunkZ - currentPosition.y) > World::renderSize / 2))
			{
				i = backDrawBuffer->erase(i);
				unloadBuffer.push_back(j);
			}
			else if (!j->needUpdate)
			{
				j->update();
				++i;
			}
		}

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