#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>

#include <GL/glew.h>
#include <SOIL.h>
#include "Shader.h"

class Skybox
{
private:
    std::vector<const GLchar*> faces;
    GLfloat size;
    GLuint textureID;
    Shader shader;
    GLuint VAO, VBO;
    GLfloat vertices[6*6*3];

    void loadCubemap();
public:
    Skybox(const GLchar* right,
           const GLchar* left,
           const GLchar* top,
           const GLchar* bottom,
           const GLchar* back,
           const GLchar* front,
           GLfloat size);
    ~Skybox();

    void draw(const glm::mat4 &view, const glm::mat4 &proj);
};
#endif