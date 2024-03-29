#include "Chunk.h"
#include "voxel.h"
#include "Lightmap.h"

Chunk::Chunk(int xpos, int zpos) : x(xpos), z(zpos) {
	voxels = new voxel[CHUNK_VOL];
	for (unsigned int i = 0; i < CHUNK_VOL; i++)
		voxels[i].id = 1;
	lightmap = new Lightmap();
	renderData.vertices = nullptr;
}

Chunk::~Chunk() {
	delete lightmap;
	delete[] voxels;
}

bool Chunk::isEmpty() {
	int id = -1;
	for (int i = 0; i < CHUNK_VOL; i++) {
		if (voxels[i].id != id) {
			if (id != -1)
				return false;
			else
				id = voxels[i].id;
		}
	}
	return true;
}

Chunk* Chunk::clone() const {
	Chunk* other = new Chunk(x, z);
	for (int i = 0; i < CHUNK_VOL; i++)
		other->voxels[i] = voxels[i];
	other->lightmap->set(lightmap);
	return other;
}

void Chunk::incref() {
	references++;
}

void Chunk::decref() {
	if (--references <= 0)
		delete this;
}