#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	glm::vec3 position, front, up, right, worldUp;

	GLfloat yaw, pitch;

	void recalculate();
public:
	Camera(glm::vec3 position,
		   glm::vec3 up);
	~Camera() {};

	glm::mat4 getViewMatrix();
	glm::vec3 getCameraPosition();

	void handleKeyboard(bool keys[], GLfloat dt);
	void handleMouse(GLfloat dx, GLfloat dy);
};
#endif

