#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>

#include "Chunk.h"
#include "Player.h"
#include "GLTexture.h"
#include "ThreadSafeWrapper.hpp"

class World
{
public:
	static constexpr Chunk::Position renderSize = 2;

	World();

	void removeAll();

	//render chunks
	void draw();
	void drawDebug();

	void updateCurrentChunkPosition();
	void unloadDistantChunks();

	void enableUpdateThread();
	void disableUpdateThread();

	//Utility functions
	Chunk * const getCurrentChunk();	
	void loadChunk(Chunk::PosVec position);
	bool chunkOutsideRenderZone(const Chunk &chunk, const Chunk::PosVec &centerChunkPosition, const int &renderSize);
	
	Block::Type getBlock(const Block::GlobalPosVec &pos);
	Block::Type *findBlock(const Block::GlobalPosVec pos);
	bool setBlock(const Block::GlobalPosVec &pos, const Block::Type &type);
	void updateChunkForBlock(const Block::PosVec &localPos, Chunk* chunk);

private:

	std::vector<Chunk*> frontDrawBuffer, backDrawBuffer;
	std::list<Chunk*> unloadBuffer;

	std::mutex frontBufferLock, unloadBufferLock, chunkMapLock;

	struct KeyHasher
	{
		std::size_t operator()(const Chunk::PosVec& key) const
		{
			return ((key.x * 5209) ^ (key.y * 1811));
		}
	};

	ThreadSafeWrapper<std::unordered_map <Chunk::PosVec, Chunk*, KeyHasher>> chunkMap;

	//chunk position is always up-to-date
	ThreadSafeWrapper<Chunk::PosVec> currentChunkPosition;
	//chunk pointer need to be checked everytime
	Chunk * currentChunk;

	//Update thread
	std::thread updateThread;
	std::atomic_bool updateThreadShouldClose;
	void updateWorldLoop();

};

inline bool World::chunkOutsideRenderZone(const Chunk& chunk, const Chunk::PosVec &centerChunkPosition, const int &renderSize)
{
	return (abs(chunk.chunkX - centerChunkPosition.x) > renderSize) || (abs(chunk.chunkZ - centerChunkPosition.y) > renderSize);
}