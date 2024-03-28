#pragma once
#include <vector>
#include "asset.hpp"
#include "mesh.hpp"

struct ModelAsset : Asset
{
	unsigned int glid;
	int vertexCount;
	std::vector<Mesh> meshes;
	static ModelAsset *loadFromDisk(const char *filepath);
};
