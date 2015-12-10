#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <memory>

#include <GL/glew.h>
#include "Maths.h"
#include "Uniform.h"

class Shader
{
private:
	const GLchar *vertexPath;
	const GLchar *fragmentPath;

	std::map<const GLchar*, std::unique_ptr<UniformData>> uniforms;

	std::string readFile(const GLchar* path);

	void attachShaders();
	void reattachShadersIfNewer();
	int compileShader(int shaderType, const GLchar* shaderSource, GLuint& shaderID);
	int Shader::checkShaderLinkingError(int shaderProgram);
public:
	GLuint program;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	template<class T>
	void assignUniform(const GLchar* name, T val);

	void bind();
	void unbind();
};

template<class T>
inline void Shader::assignUniform(const GLchar* name, T val)
{
	auto iter = uniforms.find(name);
	if (iter != uniforms.end()) {
		auto udsp = dynamic_cast<UniformDataSpec<T>*>(iter->second.get());

		udsp->set(val);
	}
	else {
		Uniform unf(program, name);
		std::unique_ptr<UniformData> udp(new UniformDataSpec<T>(unf, val));

		uniforms.emplace(name, std::move(udp));
	}
}

#endif