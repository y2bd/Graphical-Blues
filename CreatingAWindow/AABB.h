#ifndef AABB_H
#define AABB_H

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Uniform.h"

class AABB
{
private:
	GLuint VAO;
	GLuint createBoxVertexArray();
public:
	glm::vec3 center, halfDim;

	AABB(glm::vec3 center, glm::vec3 halfDim);
	~AABB() {};

	bool overlapsWith(AABB other);
	void draw(Shader& shader);

	void print();
};

#endif
