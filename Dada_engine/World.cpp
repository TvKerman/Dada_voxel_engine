#include "World.h"

#include "WorldFiles.h"
#include "Chunk.h"
#include "Chunks.h"

World::World(std::string name, std::string directory, int seed) : name(name), seed(seed) {
	wfile = new WorldFiles(directory, REGION_VOL * (CHUNK_VOL * 2 + 8));
}

World::~World() {
	delete wfile;
}