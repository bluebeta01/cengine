#include "rendercontext.hpp"
#include <glad/gl.h>

void RenderContext::initialize()
{
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH);
}

void RenderContext::beginRender(float viewportWidth, float viewportHeight)
{
	glViewport(0, 0, (int)viewportWidth, (int)viewportHeight);
}

void RenderContext::useCamera(Camera *camera)
{
	this->camera = camera;
}

void RenderContext::useProgram(Shader *shader)
{
	glUseProgram(shader->glid);
	this->shader = shader;
}

void RenderContext::clearBuffer()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderContext::renderModel(ModelAsset *model, glm::mat4 *transform)
{
	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 projMatrix = camera->getProjMatrix();

	glBindVertexArray(model->glid);
	for(const Mesh &mesh : model->meshes)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.glid);
		glUniformMatrix4fv(shader->viewMatrixLocation, 1, false, (float*)&viewMatrix);
		glUniformMatrix4fv(shader->projectionMatrixLocation, 1, false, (float*)&projMatrix);
		glUniformMatrix4fv(shader->modelMatrixLocation, 1, false, (float*)transform);
		glDrawElements(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, 0);
	}
}
