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

#include <random>

#include <math.h>

#include "Shader.h"
#include "Camera.h"
#include "Maths.h"
#include "Model.h"
#include "Mesh.h"
#include "AABB.h"
#include "Skybox.h"
#include "Sound.h"

GLFWwindow* setupWindow(int width, int height, const char* name);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void RenderQuad();

GLuint createTexture(const char* filename);

const GLfloat WIDTH = 1366, HEIGHT = 768;
const GLchar* TITLE = "Graphical Blues (167)";

unsigned int walkSound, desertMusic, grassmusic;
// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// setup camera
Camera cam(
	glm::vec3(0, -1, 4),
	glm::vec3(0, 1, 0));


// setup input
bool keys[1024];
bool mouseNotMoved = true;
bool keyIsPressed = false;
GLfloat prevX, prevY;

bool drawBoundingBoxes = false;
bool wasInGrassWorld = true;
bool inGrassWorld = false;

bool showNormal = false;
bool showNormalMap = false;
bool showShadow = false;
bool showShadowMap = false;

bool showLights = false;

bool spinLights = false;

int main()
{
	GLFWwindow* window = setupWindow(WIDTH, HEIGHT, TITLE);
	if (window == nullptr) return -1;

	// setup shaders
	Shader shader("vert.glsl", "frag.glsl");
	Shader shadowShader("shadowVert.glsl", "shadowFrag.glsl");
	Shader debugShader("debugVert.glsl", "debugFrag.glsl");
	Shader bbShader("bbVert.glsl", "bbFrag.glsl");
	Shader groundShader("groundVert.glsl", "groundFrag.glsl");
	Shader portalShader("portalVert.glsl", "portalFrag.glsl");

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
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
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

	GLuint textureGround = createTexture("Sand_6_Diffuse.png");
	GLuint textureGroundNormal = createTexture("Sand_6_Normal.png");

	GLuint textureGrass = createTexture("06_DIFFUSE.jpg");
	GLuint textureGrassNormal = createTexture("06_NORMAL.jpg");

	GLuint texturePortal = createTexture("wallpaper.jpg");

	// 3darize
	glm::mat4 model = glm::mat4();
	glm::mat4 view = cam.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), WIDTH / HEIGHT, 0.1f, 100.0f);

	// Light 
	glm::vec3 lightPos(0, 15.0f, -5.0f);
	glm::vec3 lightDir(0, -2.5f, -2.5f);
	glm::vec3 boxColor(0.2f, 0.4f, 0.8f);
	glm::vec3 gndColor(0.4f, 0.2f, 0.06f);

	glm::vec3 camPos = cam.getCameraPosition();

	glm::vec3 groundPos(0.0f, -2.0f, 0.0f);

	const int numCacti = 100;
	glm::vec3 cactiPositions[numCacti];
	GLfloat cactiRotations[numCacti];

	std::random_device rd;
	std::mt19937 gen(rd());
	gen.seed(167);
	std::uniform_real_distribution<> dis(-40, 40);
	std::uniform_real_distribution<> ang(0, 360);

	for (int i = 0; i < numCacti; i++) {

		while (true) {
			GLfloat x = dis(gen);
			GLfloat z = dis(gen);

			GLfloat dist = sqrt(x*x + z*z);

			// nothing within 1 units of center
			if (dist < 2.2f) {
				continue;
			}

			cactiPositions[i] = glm::vec3(x, -2.0f, z);
			cactiRotations[i] = glm::radians(ang(gen));

			break;
		}
	}

	// we need shadows, let's build a depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const GLfloat SHADOW_WIDTH = 1366 * 4, SHADOW_HEIGHT = 768 * 4;

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

	// grass portal buffer
	GLuint grassPortalFBO;
	glGenFramebuffers(1, &grassPortalFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, grassPortalFBO);

	GLuint grassPortal;
	glGenTextures(1, &grassPortal);
	glBindTexture(GL_TEXTURE_2D, grassPortal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, grassPortal, 0);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint grassPortalRBO;
	glGenRenderbuffers(1, &grassPortalRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, grassPortalRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, grassPortalRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// try loadin a model you fool
	Model cactusModel = Model("cactus1.obj");
	Model treeModel = Model("tree.obj");
	Model tunnelModel = Model("wall.obj");

	// calculate bounding boxes
	std::vector<AABB> cactusBoundingBoxes;
	std::vector<AABB> treeBoundingBoxes;

	for (GLuint i = 0; i < numCacti; i++) {
		glm::mat4 model;
		model = glm::translate(model, cactiPositions[i]);
		model = glm::rotate(model, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

		AABB bb = cactusModel.getBoundingBox(model);
		AABB tt = treeModel.getBoundingBox(model);

		cactusBoundingBoxes.push_back(bb);
		treeBoundingBoxes.push_back(tt);
	}

	std::vector<bool> collidedWithCacti(cactusBoundingBoxes.size());
	std::vector<bool> collidedWithTree(cactusBoundingBoxes.size());

	// character bounding box
	AABB character(cam.getCameraPosition(), glm::vec3(0.25f, 1.0f, 0.25f));

	bool transferred = false;

	// skybox thanks harvey
	Skybox nightbox("skybox/sor_hills/right.jpg", "skybox/sor_hills/left.jpg", "skybox/sor_hills/top.jpg", "skybox/sor_hills/bottom.jpg", "skybox/sor_hills/back.jpg", "skybox/sor_hills/front.jpg", 1.0f);
	Skybox skybox("skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/back.jpg", "skybox/front.jpg", 1.0f);

    Sound::InitEngine();

	// event loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (spinLights) {
			lightDir = glm::vec3(1.0f * glm::cos(currentFrame * glm::radians(50.0f)), -1.0f, 2.0f * glm::sin(currentFrame * glm::radians(50.0f)));
		}
		else {
			lightDir = glm::vec3(0, -2.5f, -2.5f);
		}

		// check, call events
		glfwPollEvents();

		// handle movement collision
		glm::vec3 oldPosition = cam.getCameraPosition();

		cam.handleKeyboard(keys, deltaTime);
		character.center = cam.getCameraPosition();

		bool collided = false;
		AABB boxThatCollided(character);
		for (int i = 0; i < cactusBoundingBoxes.size(); i++)
		{
			collidedWithCacti[i] = false;

			auto bb = cactusBoundingBoxes[i];

			if (bb.overlapsWith(character)) {
				collided = true;
				collidedWithCacti[i] = true;
				boxThatCollided = bb;
				break;
			}
		}

		for (int i = 0; i < treeBoundingBoxes.size(); i++)
		{
			collidedWithTree[i] = false;

			auto tt = treeBoundingBoxes[i];

			if (tt.overlapsWith(character)) {
				collided = true;
				collidedWithTree[i] = true;
				boxThatCollided = tt;
				break;
			}
		}

		if (collided)
		{
			glm::vec3 diff = character.center - boxThatCollided.center;
			diff.y = 0;
			glm::vec3 offset = diff * 0.00001f;

			cam.setCameraPosition(oldPosition + offset);
			character.center = oldPosition + offset;
		}
		float len = glm::distance(character.center, glm::vec3(0, -1, 0));
		if (!transferred && len < 0.5f && cam.getCameraPosition().x > 0.0) {
			inGrassWorld = !inGrassWorld;
			transferred = true;
		}

        // bgm
        if (wasInGrassWorld == true && inGrassWorld == false)
        {
            if (grassmusic != 0)             Sound::stop(grassmusic);
            grassmusic = 0;
            desertMusic = Sound::playInLoop("desertmusic.ogg");
        }
        else if (wasInGrassWorld == false && inGrassWorld == true)
        {
            if (desertMusic != 0) Sound::stop(desertMusic);
            desertMusic = 0;
            grassmusic = Sound::playInLoop("grassmusic.ogg");
        }
        wasInGrassWorld = inGrassWorld;

		if (transferred && len > 3.0f) {
			transferred = false;
		}

		lightPos = character.center;
		lightPos.y += 15.0f;

		/// BEGIN PORTAL RENDER

		if (!inGrassWorld) {
			glBindFramebuffer(GL_FRAMEBUFFER, grassPortalFBO);
			glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 modle;

			view = cam.getViewMatrix();
			camPos = cam.getCameraPosition();

			shader.assignUniform("view", view);
			shader.assignUniform("projection", proj);

			groundShader.assignUniform("view", view);
			groundShader.assignUniform("projection", proj);

			// texture 

			// light setup
			shader.assignUniform("cameraPos", camPos);
			groundShader.assignUniform("cameraPos", camPos);

			shader.assignUniform("light.position", lightDir);
			shader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			shader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			shader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			groundShader.assignUniform("light.position", lightDir);
			groundShader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			groundShader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			groundShader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			shader.assignUniform("material.ambient", boxColor);
			shader.assignUniform("material.diffuse", boxColor);
			shader.assignUniform("material.specular", glm::vec3(1, 1, 1));
			shader.assignUniform("material.shininess", 16.0f);

			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				// adjust normal
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

				shader.assignUniform("model", modle);
				shader.assignUniform("normalMatrix", normalMatrix);

				treeModel.draw(shader);
			}

			shader.assignUniform("material.ambient", glm::vec3(0.5, 0.5, 0.5));
			shader.assignUniform("material.diffuse", glm::vec3(0.7, 0.7, 0.7));
			shader.assignUniform("material.specular", glm::vec3(0, 0, 0));
			shader.assignUniform("material.shininess", 128.0f);
			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.1f, 0));
			shader.assignUniform("model", modle);
			tunnelModel.draw(shader);

			glBindVertexArray(VAO);
			// and render the floor

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureGrass);
			groundShader.assignUniform("material.texture_diffuse1", 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureGrassNormal);
			groundShader.assignUniform("normalMap", 2);

			groundShader.assignUniform("material.ambient", glm::vec3(.5, .5, .5));
			groundShader.assignUniform("material.diffuse", gndColor);
			groundShader.assignUniform("material.specular", glm::vec3(.1, .1, .1));
			groundShader.assignUniform("material.shininess", 4.0f);

			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

			groundShader.assignUniform("model", modle);
			groundShader.assignUniform("normalMatrix", normalMatrix);

			groundShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			groundShader.unbind();

			glBindVertexArray(0);

			nightbox.draw(view, proj);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			/// BEGIN SHADOW RENDER
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glm::mat4 lProjection, lView, lSpace;
			GLfloat near = 0.1f, far = 40.0f;
			lProjection = glm::ortho(-20.0f, 80.0f, -20.0f, 80.0f, near, far);
			lView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(1.0f, 0.0f, 0));
			lSpace = lProjection * lView;

			shadowShader.assignUniform("lightSpaceMatrix", lSpace);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			// simple drawing
			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				shadowShader.assignUniform("model", modle);

				cactusModel.draw(shadowShader);
			}

			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.6f, 0));
			shadowShader.assignUniform("model", modle);
			tunnelModel.draw(shadowShader);

			// and render the floor
			glBindVertexArray(VAO);
			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));

			shadowShader.assignUniform("model", modle);

			shadowShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			shadowShader.unbind();

			glBindVertexArray(0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			/// END SHADOW RENDER

			// clear
			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			/*debugShader.assignUniform("near_plane", near);
			debugShader.assignUniform("far_plane", far);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			debugShader.assignUniform("depthMap", 0);

			debugShader.bind();
			RenderQuad();
			debugShader.unbind();*/

			///// BEGIN REAL RENDER
			////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			//// just for u bb ;)
			//glLineWidth(10.0f);

			//// cam setup 
			view = cam.getViewMatrix();
			camPos = cam.getCameraPosition();

			bbShader.assignUniform("view", view);
			bbShader.assignUniform("projection", proj);

			portalShader.assignUniform("view", view);
			portalShader.assignUniform("projection", proj);

			shader.assignUniform("view", view);
			shader.assignUniform("projection", proj);
			shader.assignUniform("lightSpaceMatrix", lSpace);

			groundShader.assignUniform("view", view);
			groundShader.assignUniform("projection", proj);
			groundShader.assignUniform("lightSpaceMatrix", lSpace);

			// texture 
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			shader.assignUniform("shadowMap", 1);
			groundShader.assignUniform("shadowMap", 1);

			// light setup
			shader.assignUniform("doLight", showLights);
			groundShader.assignUniform("doLight", showLights);

			shader.assignUniform("cameraPos", camPos);
			groundShader.assignUniform("cameraPos", camPos);

			shader.assignUniform("doShadow", showShadow);
			groundShader.assignUniform("doShadow", showShadow);

			shader.assignUniform("light.position", lightDir);
			shader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			shader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			shader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			groundShader.assignUniform("light.position", lightDir);
			groundShader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			groundShader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			groundShader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			shader.assignUniform("material.ambient", boxColor);
			shader.assignUniform("material.diffuse", boxColor);
			shader.assignUniform("material.specular", glm::vec3(1, 1, 1));
			shader.assignUniform("material.shininess", 16.0f);

			// model = glm::rotate(model, deltaTime * glm::radians(50.0f), glm::vec3(-.5, 1, -1));

			GLuint redPlayer = 0;
			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				// adjust normal
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

				shader.assignUniform("model", modle);
				shader.assignUniform("normalMatrix", normalMatrix);

				cactusModel.draw(shader);

				GLuint cw = collidedWithCacti[i] ? 1 : 0;
				if (cw == 1) redPlayer = 1;

				if (drawBoundingBoxes) {
					bbShader.assignUniform("colliding", cw);
					cactusBoundingBoxes[i].draw(bbShader);
				}
			}

			if (drawBoundingBoxes) {
				bbShader.assignUniform("colliding", redPlayer);
				character.draw(bbShader);
			}

			shader.assignUniform("material.ambient", glm::vec3(0.5, 0.5, 0.5));
			shader.assignUniform("material.diffuse", glm::vec3(0.7, 0.7, 0.7));
			shader.assignUniform("material.specular", glm::vec3(0, 0, 0));
			shader.assignUniform("material.shininess", 128.0f);
			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.1f, 0));
			shader.assignUniform("model", modle);
			tunnelModel.draw(shader);

			glBindVertexArray(VAO);
			// and render the floor

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureGround);
			groundShader.assignUniform("material.texture_diffuse1", 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureGroundNormal);
			groundShader.assignUniform("normalMap", 2);
			groundShader.assignUniform("doNormal", showNormal);


			groundShader.assignUniform("material.ambient", glm::vec3(.5, .5, .5));
			groundShader.assignUniform("material.diffuse", gndColor);
			groundShader.assignUniform("material.specular", glm::vec3(.1, .1, .1));
			groundShader.assignUniform("material.shininess", 4.0f);

			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));
			normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

			groundShader.assignUniform("model", modle);
			groundShader.assignUniform("normalMatrix", normalMatrix);

			groundShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			groundShader.unbind();

			// draw portal

			if (cam.getCameraPosition().x > 0.0 && !transferred) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, grassPortal);
				portalShader.assignUniform("portalTexture", 0);

				modle = glm::mat4();
				modle = glm::translate(modle, glm::vec3(0.0f, -1.0f, 0.0));
				modle = glm::scale(modle, glm::vec3(0.1f, 2.7f, 3.2f));
				portalShader.assignUniform("model", modle);

				portalShader.bind();
				glDrawArrays(GL_TRIANGLES, 0, 36);
				portalShader.unbind();
			}

			skybox.draw(view, proj);

			glBindVertexArray(0);

			// buffer

			/// END REAL RENDER
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, grassPortalFBO);
			glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 modle;

			view = cam.getViewMatrix();
			camPos = cam.getCameraPosition();

			shader.assignUniform("view", view);
			shader.assignUniform("projection", proj);

			groundShader.assignUniform("view", view);
			groundShader.assignUniform("projection", proj);

			// texture 

			// light setup
			shader.assignUniform("cameraPos", camPos);
			groundShader.assignUniform("cameraPos", camPos);

			shader.assignUniform("light.position", lightDir);
			shader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			shader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			shader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			groundShader.assignUniform("light.position", lightDir);
			groundShader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			groundShader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			groundShader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			shader.assignUniform("material.ambient", boxColor);
			shader.assignUniform("material.diffuse", boxColor);
			shader.assignUniform("material.specular", glm::vec3(1, 1, 1));
			shader.assignUniform("material.shininess", 16.0f);

			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				// adjust normal
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

				shader.assignUniform("model", modle);
				shader.assignUniform("normalMatrix", normalMatrix);

				cactusModel.draw(shader);
			}

			shader.assignUniform("material.ambient", glm::vec3(0.5, 0.5, 0.5));
			shader.assignUniform("material.diffuse", glm::vec3(0.7, 0.7, 0.7));
			shader.assignUniform("material.specular", glm::vec3(0, 0, 0));
			shader.assignUniform("material.shininess", 128.0f);
			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.1f, 0));
			shader.assignUniform("model", modle);
			tunnelModel.draw(shader);

			glBindVertexArray(VAO);
			// and render the floor

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureGround);
			groundShader.assignUniform("material.texture_diffuse1", 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureGroundNormal);
			groundShader.assignUniform("normalMap", 2);
			groundShader.assignUniform("doNormal", showNormal);

			groundShader.assignUniform("material.ambient", glm::vec3(.5, .5, .5));
			groundShader.assignUniform("material.diffuse", gndColor);
			groundShader.assignUniform("material.specular", glm::vec3(.1, .1, .1));
			groundShader.assignUniform("material.shininess", 4.0f);

			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

			groundShader.assignUniform("model", modle);
			groundShader.assignUniform("normalMatrix", normalMatrix);

			groundShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			groundShader.unbind();

			glBindVertexArray(0);

			skybox.draw(view, proj);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			/// BEGIN SHADOW RENDER
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glm::mat4 lProjection, lView, lSpace;
			GLfloat near = 0.1f, far = 40.0f;
			lProjection = glm::ortho(-20.0f, 80.0f, -20.0f, 80.0f, near, far);
			lView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(1.0f, 0.0f, 0));
			lSpace = lProjection * lView;

			shadowShader.assignUniform("lightSpaceMatrix", lSpace);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			// simple drawing
			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				shadowShader.assignUniform("model", modle);

				treeModel.draw(shadowShader);
			}

			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.6f, 0));
			shadowShader.assignUniform("model", modle);
			tunnelModel.draw(shadowShader);

			// and render the floor
			glBindVertexArray(VAO);
			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));

			shadowShader.assignUniform("model", modle);

			shadowShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			shadowShader.unbind();

			glBindVertexArray(0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			/// END SHADOW RENDER

			// clear
			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(0.1f, 0.12f, 0.14f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			/*debugShader.assignUniform("near_plane", near);
			debugShader.assignUniform("far_plane", far);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			debugShader.assignUniform("depthMap", 0);

			debugShader.bind();
			RenderQuad();
			debugShader.unbind();*/

			///// BEGIN REAL RENDER
			////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			//// just for u bb ;)
			//glLineWidth(10.0f);

			//// cam setup 
			view = cam.getViewMatrix();
			camPos = cam.getCameraPosition();

			bbShader.assignUniform("view", view);
			bbShader.assignUniform("projection", proj);

			portalShader.assignUniform("view", view);
			portalShader.assignUniform("projection", proj);

			shader.assignUniform("view", view);
			shader.assignUniform("projection", proj);
			shader.assignUniform("lightSpaceMatrix", lSpace);

			groundShader.assignUniform("view", view);
			groundShader.assignUniform("projection", proj);
			groundShader.assignUniform("lightSpaceMatrix", lSpace);

			// texture 
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			shader.assignUniform("shadowMap", 1);
			groundShader.assignUniform("shadowMap", 1);

			// light setup
			shader.assignUniform("doLight", showLights);
			groundShader.assignUniform("doLight", showLights);
			
			shader.assignUniform("cameraPos", camPos);
			groundShader.assignUniform("cameraPos", camPos);

			shader.assignUniform("doShadow", showShadow);
			groundShader.assignUniform("doShadow", showShadow);

			shader.assignUniform("light.position", lightDir);
			shader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			shader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			shader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			groundShader.assignUniform("light.position", lightDir);
			groundShader.assignUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			groundShader.assignUniform("light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
			groundShader.assignUniform("light.specular", glm::vec3(1, 1, 1));

			shader.assignUniform("material.ambient", boxColor);
			shader.assignUniform("material.diffuse", boxColor);
			shader.assignUniform("material.specular", glm::vec3(1, 1, 1));
			shader.assignUniform("material.shininess", 16.0f);

			// model = glm::rotate(model, deltaTime * glm::radians(50.0f), glm::vec3(-.5, 1, -1));

			GLuint redPlayer = 0;
			for (GLuint i = 0; i < numCacti; i++) {
				glm::mat4 modle;
				modle = glm::translate(modle, cactiPositions[i]);
				modle = glm::rotate(modle, cactiRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));

				// adjust normal
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

				shader.assignUniform("model", modle);
				shader.assignUniform("normalMatrix", normalMatrix);

				treeModel.draw(shader);

				GLuint cw = collidedWithTree[i] ? 1 : 0;
				if (cw == 1) redPlayer = 1;

				if (drawBoundingBoxes) {
					bbShader.assignUniform("colliding", cw);
					treeBoundingBoxes[i].draw(bbShader);
				}
			}

			if (drawBoundingBoxes) {
				bbShader.assignUniform("colliding", redPlayer);
				character.draw(bbShader);
			}

			shader.assignUniform("material.ambient", glm::vec3(0.5, 0.5, 0.5));
			shader.assignUniform("material.diffuse", glm::vec3(0.7, 0.7, 0.7));
			shader.assignUniform("material.specular", glm::vec3(0, 0, 0));
			shader.assignUniform("material.shininess", 128.0f);
			modle = glm::translate(glm::mat4(), glm::vec3(0, -2.1f, 0));
			shader.assignUniform("model", modle);
			tunnelModel.draw(shader);

			glBindVertexArray(VAO);
			// and render the floor

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureGrass);
			groundShader.assignUniform("material.texture_diffuse1", 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureGrassNormal);
			groundShader.assignUniform("normalMap", 2);

			groundShader.assignUniform("material.ambient", glm::vec3(.5, .5, .5));
			groundShader.assignUniform("material.diffuse", gndColor);
			groundShader.assignUniform("material.specular", glm::vec3(.1, .1, .1));
			groundShader.assignUniform("material.shininess", 4.0f);

			modle = glm::mat4();
			modle = glm::translate(modle, groundPos);
			modle = glm::scale(modle, glm::vec3(100, 0.01f, 100));
			normalMatrix = glm::mat3(glm::transpose(glm::inverse(modle)));

			groundShader.assignUniform("model", modle);
			groundShader.assignUniform("normalMatrix", normalMatrix);

			groundShader.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			groundShader.unbind();

			// draw portal

			if (cam.getCameraPosition().x > 0.0 && !transferred) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, grassPortal);
				portalShader.assignUniform("portalTexture", 0);

				modle = glm::mat4();
				modle = glm::translate(modle, glm::vec3(0.0f, -1.0f, 0.0));
				modle = glm::scale(modle, glm::vec3(0.1f, 2.7f, 3.2f));
				portalShader.assignUniform("model", modle);

				portalShader.bind();
				glDrawArrays(GL_TRIANGLES, 0, 36);
				portalShader.unbind();
			}

			glBindVertexArray(0);

			nightbox.draw(view, proj);


			// buffer

			/// END REAL RENDER
		}
		
		if (showNormalMap || showShadowMap) {
			glActiveTexture(GL_TEXTURE0);

			if (showNormalMap && inGrassWorld) glBindTexture(GL_TEXTURE_2D, textureGrassNormal);
			if (showNormalMap && !inGrassWorld) glBindTexture(GL_TEXTURE_2D, textureGroundNormal);
			if (showShadowMap) glBindTexture(GL_TEXTURE_2D, depthMap);


			debugShader.assignUniform("depthMap", 0);

			debugShader.bind();
			RenderQuad();
			debugShader.unbind();
		}

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
			0.5f,  1.0f, 0.0f,  0.0f, 1.0f,
			0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  0.5f, 1.0f,
			1.0f, 0.5f, 0.0f,  0.5f, 0.0f,
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
	//glEnable(GL_CULL_FACE);

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

	if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
		drawBoundingBoxes = !drawBoundingBoxes;
	}

	if (key == GLFW_KEY_N && action == GLFW_RELEASE) {
		showShadow = !showShadow;
	}

	if (key == GLFW_KEY_H && action == GLFW_RELEASE) {
		showShadowMap = !showShadowMap;
	}

	if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
		showNormal = !showNormal;
	}

	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		showNormalMap = !showNormalMap;
	}

	if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
		spinLights = !spinLights;
	}

	if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
		showLights = !showLights;
	}

	if (action == GLFW_PRESS) {
		keys[key] = true; keyIsPressed = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = false;

		keyIsPressed = false;
		for (int i = 0; i < 1024; i++) {
			if (keys[i]) {
				keyIsPressed = true; 
				break;
			}
		}
	}
    if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS) grassmusic = Sound::playInLoop("walk.wav");
        else if (action == GLFW_RELEASE)Sound::stop(grassmusic);
    }
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