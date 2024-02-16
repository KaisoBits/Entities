#include "shaderprogram.h"
#include <glad/glad.h>
#include <iostream>

ShaderProgram ShaderProgram::Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	int  success;
	char infoLog[1024];

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* s1 = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &s1, nullptr);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
		std::cout << "Failed vertex shader compilation. Error: \"" << infoLog << "\"\n";
		return Empty();
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* s2 = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &s2, nullptr);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
		std::cout << "Failed fragment shader compilation. Error: \"" << infoLog << "\"\n";
		return Empty();
	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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