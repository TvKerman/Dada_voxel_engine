#pragma once
#ifndef VOXELS_CHUNKSCONTROLLER_H_
#define VOXELS_CHUNKSCONTROLLER_H_

class World;
class Chunks;
class Lighting;
class WorldFiles;
class VoxelRenderer;
class ChunksLoader;

class ChunksController {
private:
	Chunks* chunks;
	Lighting* lighting;
	ChunksLoader** loaders;
	int loadersCount;
public:
	ChunksController(World* world, Chunks* chunks, Lighting* lighting);
	~ChunksController();

	ChunksLoader* getFreeLoader();
	int countFreeLoaders();
	bool loadVisible(WorldFiles* worldFiles);
	void calculateLights();
	bool _buildMeshes(VoxelRenderer* renderer, int tick);
};

#endif /* VOXELS_CHUNKSCONTROLLER_H_ */