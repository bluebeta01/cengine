#include <iostream>
#include "modelasset.hpp"
#include "collada.hpp"
#include <glad/gl.h>

ModelAsset *ModelAsset::loadFromDisk(const char *filepath)
{
	ColladaData collada;
	parseCollada(filepath, collada);
	if(!collada.ids.size() || !collada.vertexGroups.size() || !collada.triangleGroups.size())
	{
		std::cout << "Failed to parse model " << filepath << "\n";
		return nullptr;
	}

	ModelAsset *asset = new ModelAsset();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<float> vertexData;
	int elementCounter = 0;

	for(const TriangleGroup& tg : collada.triangleGroups)
	{
		std::vector<int> elements;
		for(int i = 0; i < tg.elements.size(); i += 8)
		{
			const VertexGroup *vertGroup = nullptr;
			for(const VertexGroup& vg : collada.vertexGroups)
				if(vg.name == tg.vertexSource)
					vertGroup = &vg;
			if(!vertGroup)
				continue;
			const NamedFloatArray *vertexPositions = nullptr;
			for(const NamedFloatArray& arr : collada.ids)
				if(arr.name == vertGroup->sourceId)
					vertexPositions = &arr;
			if(!vertexPositions)
				continue;
			const NamedFloatArray *normalFloats = nullptr;
			for(const NamedFloatArray& arr : collada.ids)
				if(arr.name == tg.normalSource)
					normalFloats = &arr;
			if(!normalFloats)
				continue;
			const NamedFloatArray *uvFloats = nullptr;
			for(const NamedFloatArray& arr : collada.ids)
				if(arr.name == tg.texcoordSource)
					uvFloats = &arr;
			if(!uvFloats)
				continue;
			if(i + tg.vertexOffset + 2 >= tg.elements.size())
				continue;
			if(i + tg.normalOffset + 2 >= tg.elements.size())
				continue;
			if(i + tg.texcoordOffset + 1 >= tg.elements.size())
				continue;
			int posXIndex = tg.elements[i + tg.vertexOffset + 0] * 3 + 0;
			int posYIndex = tg.elements[i + tg.vertexOffset + 1] * 3 + 1;
			int posZIndex = tg.elements[i + tg.vertexOffset + 2] * 3 + 2;
			int nrmXIndex = tg.elements[i + tg.normalOffset + 0] * 3 + 0;
			int nrmYIndex = tg.elements[i + tg.normalOffset + 1] * 3 + 1;
			int nrmZIndex = tg.elements[i + tg.normalOffset + 2] * 3 + 2;
			int uvUIndex = tg.elements[i + tg.texcoordOffset + 0] * 2 + 0;
			int uvVIndex = tg.elements[i + tg.texcoordOffset + 1] * 2 + 1;

			if(posXIndex >= vertexPositions->floats.size()) continue;
			if(posYIndex >= vertexPositions->floats.size()) continue;
			if(posZIndex >= vertexPositions->floats.size()) continue;
			if(nrmXIndex >= normalFloats->floats.size()) continue;
			if(nrmYIndex >= normalFloats->floats.size()) continue;
			if(nrmZIndex >= normalFloats->floats.size()) continue;
			if(uvUIndex >= uvFloats->floats.size()) continue;
			if(uvVIndex >= uvFloats->floats.size()) continue;

			vertexData.push_back(vertexPositions->floats[posXIndex]);
			vertexData.push_back(vertexPositions->floats[posYIndex]);
			vertexData.push_back(vertexPositions->floats[posZIndex]);
			vertexData.push_back(normalFloats->floats[nrmXIndex]);
			vertexData.push_back(normalFloats->floats[nrmYIndex]);
			vertexData.push_back(normalFloats->floats[nrmZIndex]);
			vertexData.push_back(uvFloats->floats[uvUIndex]);
			vertexData.push_back(uvFloats->floats[uvVIndex]);

			elements.push_back(elementCounter);
			elementCounter++;
		}

		asset->meshes.emplace_back();
		Mesh &mesh = asset->meshes.back();
		mesh.elementCount = elements.size();
		glCreateBuffers(1, &mesh.glid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.glid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * elements.size(), elements.data(), GL_STATIC_DRAW);

		elements.clear();
	}

	glCreateVertexArrays(1, &asset->glid);
	glBindVertexArray(asset->glid);
	unsigned int vbo = 0;
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	asset->vertexCount = vertexData.size() / 8;

	return asset;
}
