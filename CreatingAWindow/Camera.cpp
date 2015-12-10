#include <iostream>
#include "Camera.h"

const GLfloat movementSpeed = .5f;
const GLfloat mouseSensitivity = 0.25f;

Camera::Camera(
	glm::vec3 position,
	glm::vec3 up) 
{
	this->position = position;
	this->front = glm::vec3(0, 0, -1);
	this->up = this->worldUp = up;

	this->yaw = -90.0f;
	this->pitch = 0.0f;

	recalculate();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

void Camera::recalculate()
{
	GLfloat cosYaw = glm::cos(glm::radians(yaw));
	GLfloat cosPitch = glm::cos(glm::radians(pitch));
	GLfloat sinYaw = glm::sin(glm::radians(yaw));
	GLfloat sinPitch = glm::sin(glm::radians(pitch));

	// this isn't the real front, don't be fooled!
	front = glm::vec3(
		cosYaw * cosPitch,
		sinPitch,
		sinYaw * cosPitch);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	// real front must always be orthogonal to the "world" up
}

void Camera::handleKeyboard(bool keys[], GLfloat dt)
{
	GLfloat speed = movementSpeed * dt;

	glm::vec3 worldFront = glm::cross(worldUp, right);

	if (keys[GLFW_KEY_W]) {
		position += speed * worldFront;
	}
	if (keys[GLFW_KEY_S]) {
		position -= speed * worldFront;
	}
	if (keys[GLFW_KEY_A]) {
		position -= glm::normalize(right) * speed;
	}
	if (keys[GLFW_KEY_D]) {
		position += glm::normalize(right) * speed;
	}
}

void Camera::handleMouse(GLfloat dx, GLfloat dy)
{
	dx *= mouseSensitivity;
	dy *= mouseSensitivity;

	yaw += dx;
	pitch += dy;

	// lock degree
	pitch = fmaxf(-85.0f, fminf(85.0f, pitch));

	recalculate();
}