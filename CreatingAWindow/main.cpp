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
#include "Camera.h"
#include "Maths.h"
#include "Model.h"
#include "Mesh.h"

GLFWwindow* setupWindow(int width, int height, const char* name);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void RenderQuad();

GLuint createTexture(const char* filename);

const GLfloat WIDTH = 1366, HEIGHT = 768;
const GLchar* TITLE = "lrn2opengl";

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// setup camera
Camera cam(
	glm::vec3(0, -1, 2),
	glm::vec3(0, 1, 0));

// setup input
bool keys[1024];
bool mouseNotMoved = true;
GLfloat prevX, prevY;

int main()
{
	GLFWwindow* window = setupWindow(WIDTH, HEIGHT, TITLE);
	if (window == nullptr) return -1;

	// setup shaders
	Shader shader("vert.glsl", "frag.glsl");
	Shader shadowShader("shadowVert.glsl", "shadowFrag.glsl");
	Shader debugShader("debugVert.glsl", "debugFrag.glsl");

	GLfloat vertices[] = {
		// Positions           // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	GLuint texture1 = createTexture("wall.jpg");
	GLuint textureBrown = createTexture("brown.png");

	// 3darize
	glm::mat4 model = glm::mat4();
	glm::mat4 view = cam.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), WIDTH / HEIGHT, 0.1f, 100.0f);

	// Light 
	glm::vec3 lightDir(-.3, -1, -.5);
	glm::vec3 boxColor(0.2f, 0.4f, 0.8f);
	glm::vec3 gndColor(0.4f, 0.2f, 0.06f);

	glm::vec3 camPos = cam.getCameraPosition();

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  -2.0f,  0.0f),
		glm::vec3(2.0f,  -2.0f, -15.0f),
		glm::vec3(-1.5f, -2.0f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -2.0f, -3.5f),
		glm::vec3(-1.7f,  -2.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(3.0f,  -2.0f, -2.5f),
		glm::vec3(1.5f,  -2.0f, -1.5f),
		glm::vec3(-1.3f,  -2.0f, -1.5f),
		glm::vec3(0.0f,  -2.0f, 0.0f),
	};

	// we need shadows, let's build a depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const GLfloat SHADOW_WIDTH = 1366*2, SHADOW_HEIGHT = 768*2;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	// This is to allow usage of shadow2DProj function in the shader
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	// attach shadow buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// try loadin a model you fool
	Model mmodel = Model("cactus1.obj");

	// event loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		lightDir = glm::vec3(1.0f * glm::cos(currentFrame * glm::radians(50.0f)), -1.0f, 2.0f * glm::sin(currentFrame * glm::radians(50.0f)));

		// check, call events
		glfwPollEvents();
		cam.handleKeyboard(keys, deltaTime);

		/// BEGIN SHADOW RENDER
		glm::mat4 lProjection, lView, lSpace;
		GLfloat near = 0.1f, far = 25.0f;
		lProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near, far);
		lView = glm::lookAt(glm::vec3(0, 15.0f, -5.0f), glm::vec3(0, 15.0f, -5.0f) + lightDir, glm::vec3(1.0f, 0.0f, 0));
		lSpace = lProjection * lView;

		shadowShader.assignUniform("lightSpaceMatrix", lSpace);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// simple drawing
		for (GLuint i = 0; i < 10; i++) {
			glm::mat4 modle;
			modle = glm::translate(modle, cubePositions[i]);
			//modle = glm::rotate(modle, (i * glm::radians(20.0f)) + currentFrame * glm::radians(30.0f), glm::vec3(1, glm::pow(-1, i), -glm::pow(-1, i)));

			shadowShader.assignUniform("model", modle);

			shadowShader.bind();
			mmodel.draw(shader);
			shadowShader.unbind();
		}

		// and render the floor

		glBindVertexArray(VAO);
		glm::mat4 modle;
		modle = glm::translate(modle, cubePositions[10]);
		modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));

		shadowShader.assignUniform("model", modle);

		shadowShader.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		shadowShader.unbind();

		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/// END SHADOW RENDER

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		debugShader.assignUniform("near_plane", near);
		debugShader.assignUniform("far_plane", far);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		debugShader.assignUniform("depthMap", 0);

		debugShader.bind();
		RenderQuad();
		debugShader.unbind();*/

		/// BEGIN REAL RENDER

		// clear

		// cam setup 
		view = cam.getViewMatrix();
		camPos = cam.getCameraPosition();

		shader.assignUniform("view", view);
		shader.assignUniform("projection", proj);
		shader.assignUniform("lightSpaceMatrix", lSpace);

		// texture 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shader.assignUniform("shadowMap", 1);

		// light setup
		shader.assignUniform("cameraPos", camPos);

		shader.assignUniform("light.position", lightDir);
		shader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
		shader.assignUniform("light.specular", glm::vec3(1, 1, 1));

		shader.assignUniform("material.ambient", boxColor);
		shader.assignUniform("material.diffuse", boxColor);
		shader.assignUniform("material.specular", glm::vec3(1, 1, 1));
		shader.assignUniform("material.shininess", 64.0f);

		// model = glm::rotate(model, deltaTime * glm::radians(50.0f), glm::vec3(-.5, 1, -1));

		for (GLuint i = 0; i < 10; i++) {
			glm::mat4 modle;
			modle = glm::translate(modle, cubePositions[i]);
			//modle = glm::rotate(modle, (i * glm::radians(20.0f)) + currentFrame * glm::radians(30.0f), glm::vec3(1, glm::pow(-1, i), -glm::pow(-1, i)));

			// adjust normal
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

			shader.assignUniform("model", modle);
			shader.assignUniform("normalMatrix", normalMatrix);

			shader.bind();
			mmodel.draw(shader);
			shader.unbind();
		}

		glBindVertexArray(VAO);
		// and render the floor
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureBrown);
		shader.assignUniform("material.texture_diffuse1", 0);

		shader.assignUniform("material.ambient", gndColor);
		shader.assignUniform("material.diffuse", gndColor);
		shader.assignUniform("material.specular", glm::vec3(.4, .4, .4));
		shader.assignUniform("material.shininess", 32.0f);

		modle = glm::mat4();
		modle = glm::translate(modle, cubePositions[10]);
		modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));
			
		shader.assignUniform("model", modle);
		shader.assignUniform("normalMatrix", normalMatrix);

		shader.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		shader.unbind();

		glBindVertexArray(0);

		// buffer

		/// END REAL RENDER

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VBO);

	// handle exiting
	glfwTerminate();
	
	return 0;
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
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

	// depth pls
	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback); 
	glfwSetCursorPosCallback(window, mouse_callback);

	return window;
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (action == GLFW_PRESS) keys[key] = true;
	else if (action == GLFW_RELEASE) keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (mouseNotMoved) {
		prevX = xpos;
		prevY = ypos;
		mouseNotMoved = false;
	}

	GLfloat dx = xpos - prevX;
	GLfloat dy = prevY - ypos;
	prevX = xpos;
	prevY = ypos;

	cam.handleMouse(dx, dy);
}