#pragma once
#include <string>

struct Shader
{
	std::string name;
	int glid = -1;
	int modelMatrixLocation = -1;
	int viewMatrixLocation = -1;
	int projectionMatrixLocation = -1;

	static Shader *loadBasicShader();
};
