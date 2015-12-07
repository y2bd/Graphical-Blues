#include "Skybox.h"

Skybox::Skybox(const GLchar* right,
               const GLchar* left,
               const GLchar* top,
               const GLchar* bottom,
               const GLchar* back,
               const GLchar* front,
               GLfloat size)
    : shader("skybox_vert.glsl", "skybox_frag.glsl")
{
    // setup vertices
    this->size = size;
    GLfloat vertices[6*6*3] = {
        -size / 2.0f,  size / 2.0f, -size / 2.0f,
        -size / 2.0f, -size / 2.0f, -size / 2.0f,
        size / 2.0f, -size / 2.0f, -size / 2.0f,
        size / 2.0f, -size / 2.0f, -size / 2.0f,
        size / 2.0f,  size / 2.0f, -size / 2.0f,
        -size / 2.0f,  size / 2.0f, -size / 2.0f,

        -size / 2.0f, -size / 2.0f,  size / 2.0f,
        -size / 2.0f, -size / 2.0f, -size / 2.0f,
        -size / 2.0f,  size / 2.0f, -size / 2.0f,
        -size / 2.0f,  size / 2.0f, -size / 2.0f,
        -size / 2.0f,  size / 2.0f,  size / 2.0f,
        -size / 2.0f, -size / 2.0f,  size / 2.0f,

        size / 2.0f, -size / 2.0f, -size / 2.0f,
        size / 2.0f, -size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f, -size / 2.0f,
        size / 2.0f, -size / 2.0f, -size / 2.0f,

        -size / 2.0f, -size / 2.0f,  size / 2.0f,
        -size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f, -size / 2.0f,  size / 2.0f,
        -size / 2.0f, -size / 2.0f,  size / 2.0f,

        -size / 2.0f,  size / 2.0f, -size / 2.0f,
        size / 2.0f,  size / 2.0f, -size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        size / 2.0f,  size / 2.0f,  size / 2.0f,
        -size / 2.0f,  size / 2.0f,  size / 2.0f,
        -size / 2.0f,  size / 2.0f, -size / 2.0f,

        -size / 2.0f, -size / 2.0f, -size / 2.0f,
        -size / 2.0f, -size / 2.0f,  size / 2.0f,
        size / 2.0f, -size / 2.0f, -size / 2.0f,
        size / 2.0f, -size / 2.0f, -size / 2.0f,
        -size / 2.0f, -size / 2.0f,  size / 2.0f,
        size / 2.0f, -size / 2.0f,  size / 2.0f
    };
    memcpy(this->vertices, vertices, sizeof(vertices));

    // setup skybox VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // load faces of skybox
    faces.push_back(right);
    faces.push_back(left);
    faces.push_back(top);
    faces.push_back(bottom);
    faces.push_back(back);
    faces.push_back(front);

    loadCubemap();
}

Skybox::~Skybox()
{
}

void Skybox::loadCubemap()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int width, height;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
            );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    this->textureID = textureID;
}

void Skybox::draw(const glm::mat4 &view,  const glm::mat4 &proj)
{
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
    shader.bind();
    shader.assignUniform("view", glm::mat4(glm::mat3(view)));
    shader.assignUniform("projection", proj);
    // skybox cube
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    shader.assignUniform("skybox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.unbind();
    glDepthFunc(GL_LESS); // Set depth function back to default
}