#include "HeightMap.h"

HeightMap::HeightMap()
{
}

HeightMap::~HeightMap()
{
    terrainDestroy();
}

bool HeightMap::allocateMemory()
{
    // Allocate memory for terrain heights
    if ((terrainHeights = (GLfloat *)malloc(terrainGridWidth * terrainGridLength * sizeof(GLfloat))) == nullptr)
    {
        return false;
    }
    memset(terrainHeights, 0, terrainGridWidth * terrainGridLength * sizeof(GLfloat));

    // Allocate memory for terrain normals
    if ((terrainVertices = (GLfloat *)malloc(terrainGridWidth * terrainGridLength * sizeof(GLfloat) * 3)) == nullptr)
    {
        free(terrainHeights);
        return false;
    }
    if ((terrainNormals = (GLfloat *)malloc(terrainGridWidth * terrainGridLength * sizeof(GLfloat) * 3)) == nullptr)
    {
        free(terrainVertices);
        free(terrainHeights);
        return false;
    }

    return true;
}

void HeightMap::terrainDestroy()
{
    if (terrainHeights != nullptr)
    {
        free(terrainHeights);
        free(terrainVertices);
        free(terrainNormals);
    }
}

void HeightMap::terrainSetRandomSeed(int seed)
{
    terrainRandomSeed = seed;
    srand(seed);
}

bool HeightMap::loadHeightMapFromImage(const GLchar *imagePath)
{
    // if terrain already exists, destroy it
    terrainDestroy();

    // Load image data in grayscale
    unsigned char* image = SOIL_load_image(imagePath, &terrainGridWidth, &terrainGridLength, 0, SOIL_LOAD_L);
    if (image == nullptr)
    {
        std::cerr << "Terrain error: SOIL loading error: '" << SOIL_last_result() << "'" << std::endl;
        return false;
    }

    // Allocate memory for heights and normals
    if (allocateMemory() == false)
    {
        std::cerr << "Terrain error: memory problem" << std::endl;
        SOIL_free_image_data(image);
        return false;
    }

    // fill arrays
    for (int z = 0; z < terrainGridLength; z++)
    {
        for (int x = 0; x < terrainGridWidth; x++) {
            // compute the height as a value between 0.0 and 1.0
            terrainHeights[z*terrainGridWidth + x] = image[z*terrainGridWidth + x] / 256.0;
        }
    }

    computeVerticesAndNormals();

    SOIL_free_image_data(image);
    return true;
}

/* Generates a heightmap randomly using the Circles algorithm */
bool HeightMap::generateHeightMap(int numIterations)
{
    // if terrain already exists, destroy it
    terrainDestroy();
    
    terrainGridWidth = terrainGridLength = 128;

    // Allocate memory for heights and normals
    if (allocateMemory() == false)
    {
        std::cerr << "Terrain error: memory problem" << std::endl;
        return false;
    }

    const float radius = 10.0f;
    const float minHeight = 0.1f;
    const float maxHeight = 1.0f;
    const int iterThreshold = 100;

    int iteration = 0;
    for (int k = 0; k < numIterations; k++)
    {
        // Generate random coordinate and radius
        float xCoord = ((float)rand() / RAND_MAX) * terrainGridWidth;
        float zCoord = ((float)rand() / RAND_MAX) * terrainGridLength;
        float sign = rand() > (RAND_MAX >> 1) ? 1 : -1;
        float height;
        if (++iteration < iterThreshold)
            height = maxHeight + ((float)iteration / iterThreshold) * (minHeight - maxHeight);
        for (int z = floor(zCoord - radius); z < ceil(zCoord + radius); z++)
        {
            for (int x = floor(xCoord - radius); x < ceil(xCoord + radius); x++)
            {
                // Skip if out of bounds
                if (x < 0 || z < 0 || x >= terrainGridWidth || z > terrainGridLength) { continue; }

                // Calculate height
                float dist = sqrt((x - xCoord) * (x - xCoord) + (z - zCoord) * (z - zCoord));
                if (dist < radius)
                {
                    //terrainHeights[z*terrainGridWidth + x] += sign * height / 2 * (1 + cos(dist/radius * glm::pi<float>()));
                    terrainHeights[z*terrainGridWidth + x] += sqrt(radius * radius - dist * dist) * height;
                }
            }
        }
    }
    
    // Normalize heights to [0,1]
    float min = -numIterations, max = numIterations;
    for (int i = 0; i < terrainGridWidth * terrainGridLength; i++)
    {
        if (terrainHeights[i] < min) { min = terrainHeights[i]; }
        if (terrainHeights[i] > max) { max = terrainHeights[i]; }
    }
    for (int i = 0; i < terrainGridWidth * terrainGridLength; i++)
    {
        terrainHeights[i] = (terrainHeights[i] - min) / (max - min);
    }

    // TODO: smoothing
    
    computeVerticesAndNormals();
    return true;
}

void HeightMap::computeVerticesAndNormals()
{
    for (int z = 0; z < terrainGridLength; z++)
    {
        for (int x = 0; x < terrainGridWidth; x++) {
            // x-coordinate scaled to range [-0.5,0.5]
            terrainVertices[3 * (z*terrainGridWidth + x)] = -0.5f + (float)x / (terrainGridWidth - 1);
            // y-coordinate is height
            terrainVertices[3 * (z*terrainGridWidth + x) + 1] = terrainHeights[z*terrainGridWidth + x];
            // z-coordinate scaled to range [-0.5,0.5]
            terrainVertices[3 * (z*terrainGridWidth + x) + 2] = -0.5f + (float)z / (terrainGridWidth - 1);
        }
    }
    // TODO: computeNormals
}

void HeightMap::draw()
{

}