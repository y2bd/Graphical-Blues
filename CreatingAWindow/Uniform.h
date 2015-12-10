#ifndef UNIFORM_H
#define UNIFORM_H

#include <GL/glew.h>
#include "Maths.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

class Uniform
{
private:
	GLint location;
public:
	Uniform(GLint program, const GLchar * name);

	template<class T>
	void apply(T value) 
	{
		printf("Using default Uniform application!! error!!\n");
	}
};

template<>
void Uniform::apply(Vector4f value);

template<>
void Uniform::apply(Vector3f value);

template<>
void Uniform::apply(glm::mat2 value);

template<>
void Uniform::apply(glm::mat3 value);

template<>
void Uniform::apply(glm::mat4 value);

template<>
void Uniform::apply(glm::vec2 value);

template<>
void Uniform::apply(glm::vec3 value);

template<>
void Uniform::apply(glm::vec4 value);

template<>
void Uniform::apply(GLuint value);

template<>
void Uniform::apply(GLint value);

template<>
void Uniform::apply(GLfloat value);


class UniformData
{
public:
	virtual void apply() { printf("Never apply a raw UniformData, only exists for default constructor.");  };
};


template<class T>
class UniformDataSpec : public UniformData
{
private:
	T data;
	Uniform uniform;
public:
	UniformDataSpec(Uniform uniform, T data);
	void set(T data);
	void apply();
};

template<class T>
inline UniformDataSpec<T>::UniformDataSpec(Uniform uniform, T data)
	: uniform(uniform),
	data(data)
{
}

template<class T>
inline void UniformDataSpec<T>::set(T data)
{
	this->data = data;
}

template<class T>
inline void UniformDataSpec<T>::apply()
{
	this->uniform.apply<T>(this->data);
}

#endif