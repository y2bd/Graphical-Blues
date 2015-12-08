#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <SOIL.h>

class HeightMap
{
private:
    int terrainGridWidth; // number of heights available on the x-axis
    int terrainGridLength; // number of heights available on the y-axis
    GLfloat *terrainHeights; // one-dimensional array containing heights
    GLfloat *terrainVertices;
    GLfloat *terrainNormals; // one-dimensional array containing normals for each height
    GLuint VAO, VBO;

    int terrainRandomSeed;

public:
    HeightMap();
    ~HeightMap();
    bool allocateMemory();
    void terrainDestroy();
    void terrainSetRandomSeed(int seed);

    bool loadHeightMapFromImage(const GLchar *imagePath);
    bool generateHeightMap(int numIterations);
    void computeVerticesAndNormals();
    void draw();
};

#endif