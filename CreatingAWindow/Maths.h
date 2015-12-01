#ifndef MATHS_H
#define MATHS_H
#include <GL/glew.h>

struct Vector4f
{
	GLfloat x, y, z, w;

	Vector4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) : x(x), y(y), z(z), w(w) {}
};

struct Vector3f
{
	GLfloat x, y, z;

	Vector3f(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};

struct Vector1i
{
	GLint i;

	Vector1i(GLint i) : i(i) {}
};
#endif