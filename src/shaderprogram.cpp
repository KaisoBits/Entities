#include "shaderprogram.h"
#include <glad/glad.h>
#include <iostream>

ShaderProgram ShaderProgram::Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	int  success;
	char infoLog[1024];

	std::cout << "Compiling shader\n";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "  Compiling vertex shader ID " << vertexShader << '\n';
	const char* s1 = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &s1, nullptr);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
		std::cout << "  Failed vertex shader compilation. Error: \"" << infoLog << "\"\n";
		return Empty();
	}
	std::cout << "  Vertex shader compiled successfully\n";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "  Compiling fragment shader ID " << fragmentShader << '\n';
	const char* s2 = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &s2, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
		std::cout << "  Failed fragment shader compilation. Error: \"" << infoLog << "\"\n";
		return Empty();
	}
	std::cout << "  Fragment shader compiled successfully\n";

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	std::cout << "  Shader linked to program with ID " << shaderProgram << '\n';

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	std::cout << "  Cleanup\n";

	return ShaderProgram(shaderProgram);
}

int ShaderProgram::GetPramLocation(const std::string& paramName) const
{
	return glGetUniformLocation(m_programId, paramName.c_str());
}

void ShaderProgram::Use() const
{
	glUseProgram(m_programId);
}

void ShaderProgram::SetInt(const std::string& paramName, int value) const
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}
	glUniform1i(location, value);
}

void ShaderProgram::SetFloat(const std::string& paramName, float value) const
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}
	glUniform1f(location, value);
}

void ShaderProgram::SetVector3(const std::string& paramName, glm::vec3 value) const
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}
	glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::SetVector2(const std::string& paramName, glm::vec2 value) const
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}
	glUniform2fv(location, 1, &value[0]);
}