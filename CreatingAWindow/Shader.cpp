#include "Shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
{
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;

	attachShaders();
}

void Shader::bind()
{
	glUseProgram(this->program);

	for (auto &item : uniforms)
	{
		item.second->apply();
	}
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::attachShaders()
{
	std::string vpContents = readFile(vertexPath);
	std::string fpContents = readFile(fragmentPath);

	GLuint vertex, fragment;
	compileShader(GL_VERTEX_SHADER, vpContents.c_str(), vertex);
	compileShader(GL_FRAGMENT_SHADER, fpContents.c_str(), fragment);

	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);

	checkShaderLinkingError(this->program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::reattachShadersIfNewer()
{

}

std::string Shader::readFile(const GLchar * path)
{
	std::string fileContents;
	std::ifstream file;

	file.exceptions(std::ifstream::badbit);
	try
	{
		file.open(path);

		std::stringstream fileStream;
		fileStream << file.rdbuf();

		file.close();

		fileContents = fileStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	return fileContents;
}

int Shader::compileShader(int shaderType, const GLchar * shaderSource, GLuint & shaderID)
{
	shaderID = glCreateShader(shaderType);

	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED" << std::endl << infoLog << std::endl;

		return -1;
	}

	return 0;
}

int Shader::checkShaderLinkingError(int shaderProgram)
{
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINKING_FAILED" << std::endl << infoLog << std::endl;

		return -1;
	}

	return 0;
}
