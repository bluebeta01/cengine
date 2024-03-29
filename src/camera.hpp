#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>

struct Camera
{
	glm::quat rotation;
	glm::vec3 position;
	float aspect = 16.0f / 9.0f;
	float speed = 1.0f;

	Camera();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjMatrix();

	void moveForward(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveBack(float speed);
	void moveUp(float speed);
	void moveDown(float speed);
	void rotate(float x, float y);
	void resizeViewport(float width, float height);
};
