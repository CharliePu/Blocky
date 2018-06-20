#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "Chunk.h"
#include "GLCamera.h"
#include "GLTexture.h"

class World
{
public:
	static constexpr Chunk::Position renderSize = 10;

	World();

	//render chunks
	void draw();
	void drawDebug();

	void enableUpdateThread();
	void disableUpdateThread();

private:

	std::list<Chunk*> *frontDrawBuffer;
	std::list<Chunk*> *backDrawBuffer;
	std::list<Chunk*> unloadBuffer;

	struct KeyHasher
	{
		std::size_t operator()(const Chunk::PosVec& key) const
		{
			return ((key.x * 5209) ^ (key.y * 1811));
		}
	};

	std::unordered_map <Chunk::PosVec, Chunk*, KeyHasher> chunkMap;
	std::mutex bufferLock;

	std::thread updateThread;
	bool updateThreadShouldClose;

	void updateWorldLoop();
	void loadChunk(Chunk::PosVec position);
};
