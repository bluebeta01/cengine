#pragma once
#include "camera.hpp"
#include "modelasset.hpp"
#include "shader.hpp"

struct RenderContext
{
	Camera *camera;
	Shader *shader;

	void initialize();
	void clearBuffer();
	void useCamera(Camera *camera);
	void useProgram(Shader *shader);
	void beginRender(float viewportWidth, float viewportHeight);
	void renderModel(ModelAsset *model, glm::mat4 *transform);
};
