#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>

#include "Shader.h"
#include "Maths.h"
#include "Skybox.h"

GLFWwindow* setupWindow(int width, int height, const char* name);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint createTexture(const char* filename);

const GLfloat WIDTH = 800, HEIGHT = 600;
const GLchar* TITLE = "lrn2opengl";

GLfloat mixer = 0.0;

int main()
{
	GLFWwindow* window = setupWindow(WIDTH, HEIGHT, TITLE);
	if (window == nullptr) return -1;

	// setup shaders
	Shader shader("vert.glsl", "frag.glsl");

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f
	};

	GLfloat floorVert[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	GLuint VBOf, VAOf;
	glGenVertexArrays(1, &VAOf);
	glGenBuffers(1, &VBOf);

	glBindVertexArray(VAOf);

	glBindBuffer(GL_ARRAY_BUFFER, VBOf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVert), floorVert, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	GLuint texture1 = createTexture("wall.jpg");
	GLuint texture2 = createTexture("wall.jpg");

	// 3darize
	glm::mat4 model; model = glm::rotate(model, glm::radians(55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view; view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 proj; proj = glm::perspective(glm::radians(90.0f), WIDTH / HEIGHT, 0.1f, 100.0f);

	glm::mat4 modelF; 
	modelF = glm::translate(modelF, glm::vec3(0, -2.0f, 1.0f));
	modelF = glm::rotate(modelF, glm::radians(55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelF = glm::scale(modelF, glm::vec3(10.0f, 1.0f, 10.0f));

	// Light source
	glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);

    // Initialize skybox
    Skybox skybox("skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/back.jpg", "skybox/front.jpg", 1.0f);

	// event loop
	while (!glfwWindowShouldClose(window))
	{
		// check, call events
		glfwPollEvents();

		// clear
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.assignUniform("lightPos", lightPos);
		shader.assignUniform("viewPos", camPos);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		shader.assignUniform("texture1", 0);

		shader.assignUniform("model", modelF);

		shader.bind();

		glBindVertexArray(VAOf);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		shader.unbind();

		shader.assignUniform("lightPos", lightPos);
		shader.assignUniform("viewPos", camPos);

		// apply texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		shader.assignUniform("texture1", 0);
		
		shader.assignUniform("mixer", mixer);

		model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(50.0f) / 5000.0f, glm::vec3(0.5f, 1.0f, 0.0f));
		shader.assignUniform("model", model);
		shader.assignUniform("view", view);
		shader.assignUniform("projection", proj);

		// apply shaders
		shader.bind();

		// draw stuff lol
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		shader.unbind();

        // draw the skybox last
        skybox.draw(view, proj);

		// buffer
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VBO);

	// handle exiting
	glfwTerminate();
	
	return 0;
}

GLFWwindow* setupWindow(int width, int height, const char* name)
{
	// init window
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	// create and assign window
	GLFWwindow* window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	// init glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to init GLEW" << std::endl;
		return nullptr;
	}

	// register screen mapping
	glViewport(0, 0, width, height);

	// register callbacks
	glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	return window;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		mixer = fminf(mixer + 0.025, 1.0);
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		mixer = fmaxf(mixer - 0.025, 0);
	}
}

GLuint createTexture(const char* filename)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}