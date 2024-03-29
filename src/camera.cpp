#include "camera.hpp"

Camera::Camera()
{
	rotation = glm::identity<glm::quat>();
	position = glm::vec3(0, 0, 0);
}

glm::mat4 Camera::getProjMatrix()
{
	return glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::mat4_cast(rotation) * glm::translate(glm::identity<glm::mat4>(), position);
}

void Camera::resizeViewport(float width, float height)
{
	aspect = width / height;
}

void Camera::moveForward(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(0, 0, 1) * speed;
}

void Camera::moveBack(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(0, 0, -1) * speed;
}

void Camera::moveLeft(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(1, 0, 0) * speed;
}

void Camera::moveRight(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(-1, 0, 0) * speed;
}

void Camera::moveUp(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(0, 1, 0) * speed;
}

void Camera::moveDown(float speed)
{
	position += glm::inverse(rotation) * glm::vec3(0, -1, 0) * speed;
}

void Camera::rotate(float x, float y)
{
	glm::quat newXRot = glm::quat(glm::vec3(x, 0, 0)) * rotation;
	glm::vec3 forward = newXRot * glm::vec3(0, 1, 0);
	if(forward.y > 0.1f)
		rotation = newXRot;
	rotation = rotation * glm::quat(glm::vec3(0, y, 0));
}
