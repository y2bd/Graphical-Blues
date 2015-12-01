#include "Uniform.h"

Uniform::Uniform(GLint program, const GLchar * name)
{
	this->location = glGetUniformLocation(program, name);
}

template<>
void Uniform::apply(Vector4f value)
{
	glUniform4f(this->location, value.x, value.y, value.z, value.w);
}

template<>
void Uniform::apply(Vector3f value)
{
	glUniform3f(this->location, value.x, value.y, value.z);
}

template<>
void Uniform::apply(glm::vec2 value)
{
	glUniform2fv(this->location, 1, glm::value_ptr(value));
}

template<>
void Uniform::apply(glm::vec3 value)
{
	glUniform3fv(this->location, 1, glm::value_ptr(value));
}

template<>
void Uniform::apply(glm::vec4 value)
{
	glUniform4fv(this->location, 1, glm::value_ptr(value));
}

template<>
void Uniform::apply(glm::mat2 value)
{
	glUniformMatrix2fv(this->location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Uniform::apply(glm::mat3 value)
{
	glUniformMatrix3fv(this->location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Uniform::apply(glm::mat4 value)
{
	glUniformMatrix4fv(this->location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Uniform::apply(GLint value)
{
	glUniform1i(this->location, value);
}

template<>
void Uniform::apply(GLfloat value)
{
	glUniform1f(this->location, value);
}
