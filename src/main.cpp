#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW3.h>
#include "shader.hpp"
#include "modelasset.hpp"
#include "camera.hpp"
#include "rendercontext.hpp"

int main()
{
	GLFWwindow *window;
	glfwInit();
	window = glfwCreateWindow(1920, 1080, "Game", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	Shader *basicShader = Shader::loadBasicShader();
	ModelAsset *model = ModelAsset::loadFromDisk("C:/game/assets/sphere.dae");
	Camera camera;
	camera.position.z = -30;
	RenderContext renderContext;
	renderContext.initialize();
	glm::mat4 identity = glm::identity<glm::mat4>();
	while(!glfwWindowShouldClose(window))
	{
		renderContext.clearBuffer();
		renderContext.beginRender(1920, 1080);
		renderContext.useCamera(&camera);
		renderContext.useProgram(basicShader);
		renderContext.renderModel(model, &identity);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
