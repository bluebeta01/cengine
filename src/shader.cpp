#include <fstream>
#include <iostream>
#include "shader.hpp"
#include <glad/gl.h>

Shader *Shader::loadBasicShader()
{
	std::ifstream stream = {};
	stream.open("assets/shaders/basic.vglsl", std::ios_base::in);
	if(!stream.is_open())
	{
		std::cout << "Failed to open basic shader vert source\n";
		return nullptr;
	}
	std::string vertFileData;
	while(!stream.eof())
	{
		std::string buffer;
		std::getline(stream, buffer);
		vertFileData += buffer;
		vertFileData += '\n';
	}
	stream.close();

	stream.open("assets/shaders/basic.fglsl", std::ios_base::in);
	if(!stream.is_open())
	{
		std::cout << "Failed to open basic shader vert source\n";
		return nullptr;
	}
	std::string fragFileData;
	while(!stream.eof())
	{
		std::string buffer;
		std::getline(stream, buffer);
		fragFileData += buffer;
		fragFileData += '\n';
	}

	int vertShader = glCreateShader(GL_VERTEX_SHADER);
	const char *vertSource = vertFileData.c_str();
	glShaderSource(vertShader, 1, &vertSource, NULL);
	glCompileShader(vertShader);
	int result;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		char infoBuffer[1000];
		int infoBufferLength;
		glGetShaderInfoLog(vertShader, 1000, &infoBufferLength, infoBuffer);
		std::cout << "Basic shader vertex compile log:\n" << infoBuffer << "\n";
		glDeleteShader(vertShader);
		return nullptr;
	}

	int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragSource = fragFileData.c_str();
	glShaderSource(fragShader, 1, &fragSource, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		char infoBuffer[1000];
		int infoBufferLength;
		glGetShaderInfoLog(fragShader, 1000, &infoBufferLength, infoBuffer);
		std::cout << "Basic shader fragment compile log:\n" << infoBuffer << "\n";
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		return nullptr;
	}

	Shader *shader = new Shader();
	shader->name = "Basic Shader";
	
	shader->glid = glCreateProgram();
	glAttachShader(shader->glid, vertShader);
	glAttachShader(shader->glid, fragShader);
	glLinkProgram(shader->glid);
	glGetProgramiv(shader->glid, GL_LINK_STATUS, &result);
	if(!result)
	{
		char infoBuffer[1000];
		int infoBufferLength;
		glGetProgramInfoLog(shader->glid, 1000, &infoBufferLength, infoBuffer);
		std::cout << "Basic shader link log:\n" << infoBuffer << "\n";
		std::cout << vertFileData << "\n";
		std::cout << fragFileData << "\n";
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		glDeleteProgram(shader->glid);
		return nullptr;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	shader->modelMatrixLocation = glGetUniformLocation(shader->glid, "model");
	shader->viewMatrixLocation = glGetUniformLocation(shader->glid, "view");
	shader->projectionMatrixLocation = glGetUniformLocation(shader->glid, "projection");

	return shader;
}
