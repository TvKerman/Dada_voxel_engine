#pragma once
#ifndef VOXELS_WORLDGENERATOR_H_
#define VOXELS_WORLDGENERATOR_H_

class voxel;

class WorldGenerator {
public:
	static void generate(voxel* voxels, int x, int z, int seed);
};

#endif /* VOXELS_WORLDGENERATOR_H_ */