#pragma once
#include <list>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>

#include "Chunk.h"
#include "Camera.h"
#include "GLTexture.h"

class World
{
public:
	static constexpr Chunk::Position renderSize = 11;

	World();

	void removeAll();

	//render chunks
	void draw();
	void drawDebug();

	void updateCurrentChunkPosition();
	void unloadDistantChunks();

	Chunk &getCurrentChunk();

	void enableUpdateThread();
	void disableUpdateThread();

	std::atomic_bool renderFinished;
private:

	std::list<Chunk*> frontDrawBuffer;
	std::list<Chunk*> backDrawBuffer;
	std::queue<Chunk*> unloadBuffer;

	struct KeyHasher
	{
		std::size_t operator()(const Chunk::PosVec& key) const
		{
			return ((key.x * 5209) ^ (key.y * 1811));
		}
	};

	std::unordered_map <Chunk::PosVec, Chunk*, KeyHasher> chunkMap;
	std::mutex bufferLock;

	Chunk::PosVec currentChunkPosition;

	std::thread updateThread;
	std::atomic_bool updateThreadShouldClose;

	void updateWorldLoop();
	void loadChunk(Chunk::PosVec position);

	bool chunkOutsideRenderZone(const Chunk &chunk, const Chunk::PosVec &centerChunkPosition);
	bool chunkOutsideRenderZone(const Chunk &chunk);
};

inline Chunk & World::getCurrentChunk()
{
	return *chunkMap[currentChunkPosition];
}

inline bool World::chunkOutsideRenderZone(const Chunk& chunk)
{
	return (abs(chunk.chunkX - currentChunkPosition.x) > renderSize) || (abs(chunk.chunkZ - currentChunkPosition.y) > renderSize);
}

inline bool World::chunkOutsideRenderZone(const Chunk& chunk, const Chunk::PosVec &centerChunkPosition)
{
	return (abs(chunk.chunkX - centerChunkPosition.x) > renderSize) || (abs(chunk.chunkZ - centerChunkPosition.y) > renderSize);
}