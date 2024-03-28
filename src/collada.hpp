#pragma once
#include <string_view>
#include <vector>

struct NamedFloatArray
{
	std::wstring_view name;
	std::vector<float> floats;
};

struct VertexGroup
{
	std::wstring_view name;
	std::wstring_view sourceId;
};

struct TriangleGroup
{
	std::wstring_view materialName;
	std::wstring_view vertexSource;
	std::wstring_view normalSource;
	std::wstring_view texcoordSource;
	int vertexOffset;
	int normalOffset;
	int texcoordOffset;
	std::vector<int> elements;
};

struct ColladaData
{
	std::vector<NamedFloatArray> ids;
	std::vector<VertexGroup> vertexGroups;
	std::vector<TriangleGroup> triangleGroups;
	wchar_t *daeData = NULL;
	wchar_t *daeDataEnd = NULL;

	~ColladaData();
};

extern void parseCollada(const char *filepath, ColladaData& collada);
