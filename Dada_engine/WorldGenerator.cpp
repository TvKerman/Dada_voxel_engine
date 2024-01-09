#include "WorldGenerator.h"
#include "voxel.h"
#include "Chunk.h"

#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#define FNL_IMPL
#include "FastNoiseLite.h"
#include <time.h>

#include "declarations.h"


class PseudoRandom {
	unsigned short seed;
public:
	PseudoRandom() {
		seed = (unsigned short)time(0);
	}

	int rand() {
		seed = (seed + 0x7ed5 + (seed << 6));
		seed = (seed ^ 0xc23c ^ (seed >> 9));
		seed = (seed + 0x1656 + (seed << 3));
		seed = ((seed + 0xa264) ^ (seed << 4));
		seed = (seed + 0xfd70 - (seed << 3));
		seed = (seed ^ 0xba49 ^ (seed >> 8));

		return (int)seed;
	}

	void setSeed(int number) {
		seed = ((unsigned short)number + 23729 xor (unsigned short)number + 16786);
		rand();
	}
};

float calc_height(fnl_state* noise, int real_x, int real_z) {
	float height = fnlGetNoise3D(noise, real_x * 0.0125f * 8, real_z * 0.0125f * 8, 0.0f);
	height += fnlGetNoise3D(noise, real_x * 0.025f * 8, real_z * 0.025f * 8, 0.0f) * 0.5f;
	height += fnlGetNoise3D(noise, real_x * 0.05f * 8, real_z * 0.05f * 8, 0.0f) * 0.25f;
	height += fnlGetNoise3D(noise,
		real_x * 0.2f * 8 + fnlGetNoise3D(noise, real_x * 0.1f * 8, real_z * 0.1f * 8, 0.0f) * 50,
		real_z * 0.2f * 8 + fnlGetNoise3D(noise, real_x * 0.1f * 8 + 4363, real_z * 0.1f * 8, 0.0f) * 50,
		0.0f) * 0.1f;
	height += fnlGetNoise3D(noise, real_x * 0.1f * 8, real_z * 0.1f * 8, 0.0f) * 0.125f;
	height += fnlGetNoise3D(noise, real_x * 0.4f * 8, real_z * 0.4f * 8, 0.0f) * 0.0625f;
	height += fnlGetNoise3D(noise, real_x * 8, real_z * 8, 0.0f) * 0.03f * (fnlGetNoise3D(noise, -real_x * 0.0125f * 8 - 1000, real_z * 0.0125f * 8 + 2000, 0.0f) / 2 + 0.5f);
	height *= fnlGetNoise3D(noise, real_x * 0.0125f * 8 + 1000, real_z * 0.0125f * 8 + 1000, 0.0f) / 2 + 0.5f;
	height += 1.0f;
	height *= 64.0f;
	return height;
}

float calc_height_faster(fnl_state* noise, int real_x, int real_z) {
	float height = fnlGetNoise3D(noise, real_x * 0.0125f * 8, real_z * 0.0125f * 8, 0.0f);
	height += fnlGetNoise3D(noise, real_x * 0.025f * 8, real_z * 0.025f * 8, 0.0f) * 0.5f;
	height += fnlGetNoise3D(noise, real_x * 0.05f * 8, real_z * 0.05f * 8, 0.0f) * 0.25f;
	height += fnlGetNoise3D(noise,
		real_x * 0.2f * 8 + fnlGetNoise3D(noise, real_x * 0.1f * 8, real_z * 0.1f * 8, 0.0f) * 50,
		real_z * 0.2f * 8 + fnlGetNoise3D(noise, real_x * 0.1f * 8 + 4363, real_z * 0.1f * 8, 0.0f) * 50,
		0.0f) * 0.1f;
	height += fnlGetNoise3D(noise, real_x * 0.1f * 8, real_z * 0.1f * 8, 0.0f) * 0.125f;
	height *= fnlGetNoise3D(noise, real_x * 0.0125f * 8 + 1000, real_z * 0.0125f * 8 + 1000, 0.0f) / 2 + 0.5f;
	height += 1.0f;
	height *= 64.0f;
	return height;
}

int generate_tree(fnl_state* noise, PseudoRandom* random, const float* heights, int real_x, int real_y, int real_z, int tileSize) {
	const int tileX = floor((double)real_x / (double)tileSize);
	const int tileY = floor((double)real_z / (double)tileSize);
	random->setSeed(tileX * 4325261 + tileY * 12160951 + tileSize * 9431111);

	bool gentree = fnlGetNoise3D(noise, tileX * 3.0f + 633, 0.0, tileY * 3.0f) > -0.1f && (random->rand() % 10) < 7;
	if (!gentree)
		return 0;

	const int randomX = (random->rand() % (tileSize / 2)) - tileSize / 4;
	const int randomZ = (random->rand() % (tileSize / 2)) - tileSize / 4;
	int centerX = tileX * tileSize + tileSize / 2 + randomX;
	int centerY = tileY * tileSize + tileSize / 2 + randomZ;
	int height = (int)calc_height_faster(noise, centerX, centerY);
	if ((height < 57) || (fnlGetNoise3D(noise, real_x * 0.025f, real_z * 0.025f, 0.0f) * 0.5f > 0.5))
		return 0;
	int lx = real_x - centerX;
	int radius = random->rand() % 4 + 3;
	int ly = real_y - height - 3 * radius;
	int lz = real_z - centerY;
	if (lx == 0 && lz == 0 && real_y - height < 4 * radius)
		return 6;
	if (lx * lx + ly * ly / 2 + lz * lz < radius * radius)
		return 7;
	return 0;
}

void WorldGenerator::generate(voxel* voxels, int cx, int cz, int seed) {
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.seed = seed * 60617077 % 25896307;
	PseudoRandom randomtree;

	float heights[CHUNK_VOL];

	for (int z = 0; z < CHUNK_D; z++) {
		for (int x = 0; x < CHUNK_W; x++) {
			int real_x = x + cx * CHUNK_W;
			int real_z = z + cz * CHUNK_D;
			float height = calc_height(&noise, real_x, real_z);
			heights[z * CHUNK_W + x] = height;
		}
	}

	for (int z = 0; z < CHUNK_D; z++) {
		int real_z = z + cz * CHUNK_D;
		for (int x = 0; x < CHUNK_W; x++) {
			int real_x = x + cx * CHUNK_W;
			float height = heights[z * CHUNK_W + x];

			for (int y = 0; y < CHUNK_H; y++) {
				int real_y = y;
				int id = real_y < 55 ? BLOCK_WATER : BLOCK_AIR;
				if ((real_y == (int)height) && (54 < real_y)) {
					id = BLOCK_GRASS_BLOCK;
				}
				else if (real_y < (height - 6)) {
					id = BLOCK_STONE;
				}
				else if (real_y < height) {
					id = BLOCK_DIRT;
				}
				else {
					int tree = generate_tree(&noise, &randomtree, heights, real_x, real_y, real_z, 16);
					if (tree) {
						id = tree;
					}
					else if ((tree = generate_tree(&noise, &randomtree, heights, real_x, real_y, real_z, 19))) {
						id = tree;
					}
					else if ((tree = generate_tree(&noise, &randomtree, heights, real_x, real_y, real_z, 23))) {
						id = tree;
					}
				}
				if (((height - (1.5 - 0.2 * pow(height - 54, 4))) < real_y) && (real_y < height)) {
					id = BLOCK_SAND;
				}
				if (real_y <= 2)
					id = BLOCK_BEDROCK;
				if ((id == 0) && (real_y > 55) && ((int)height + 1 == real_y) && ((unsigned short)randomtree.rand() > 56000)) {
					id = BLOCK_GRASS;
				}
				if ((id == 0) && (real_y > 55) && ((int)height + 1 == real_y) && ((unsigned short)randomtree.rand() > 64000)) {
					id = BLOCK_FLOWER;
				}
				voxels[(y * CHUNK_D + z) * CHUNK_W + x].id = id;
			}
		}
	}
}