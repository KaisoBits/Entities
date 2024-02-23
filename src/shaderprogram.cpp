#include "shaderprogram.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

unsigned int ShaderProgram::m_currentlyUsedShader = 0;

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
	auto iter = m_shaderLocationCache.find(paramName);
	int value = -1;
	if (iter != m_shaderLocationCache.end())
	{
		value = iter->second;
	}
	else
	{
		value = glGetUniformLocation(m_programId, paramName.c_str());
		m_shaderLocationCache[paramName] = value;
	}

	return value;
}

void ShaderProgram::Use() const
{
	unsigned int id = GetId();
	if (m_currentlyUsedShader != id)
	{
		m_currentlyUsedShader = id;
		glUseProgram(m_programId);
	}
}

void ShaderProgram::SetInt(const std::string& paramName, int value)
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		if (m_verboseLogging)
			std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}

	auto cachedIt = m_shaderValueCache.find(paramName);
	if (cachedIt == m_shaderValueCache.end() || std::get<int>(cachedIt->second) != value) {
		m_shaderValueCache[paramName] = value;
		glUniform1i(location, value);
	}
}

void ShaderProgram::SetFloat(const std::string& paramName, float value)
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		if (m_verboseLogging)
			std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}

	auto cachedIt = m_shaderValueCache.find(paramName);
	if (cachedIt == m_shaderValueCache.end() || std::get<float>(cachedIt->second) != value)
	{
		m_shaderValueCache[paramName] = value;
		glUniform1f(location, value);
	}
}

void ShaderProgram::SetVector3(const std::string& paramName, const glm::vec3& value)
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		if (m_verboseLogging)
			std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}

	auto cachedIt = m_shaderValueCache.find(paramName);
	if (cachedIt == m_shaderValueCache.end() || std::get<glm::vec3>(cachedIt->second) != value)
	{
		m_shaderValueCache[paramName] = value;
		glUniform3fv(location, 1, &value[0]);
	}
}

void ShaderProgram::SetVector2(const std::string& paramName, const glm::vec2& value)
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		if (m_verboseLogging)
			std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}

	auto cachedIt = m_shaderValueCache.find(paramName);
	if (cachedIt == m_shaderValueCache.end() || std::get<glm::vec2>(cachedIt->second) != value)
	{
		m_shaderValueCache[paramName] = value;
		glUniform2fv(location, 1, &value[0]);
	}
}

void ShaderProgram::SetMat4(const std::string& paramName, const glm::mat4& value)
{
	int location = GetPramLocation(paramName);
	if (location < 0)
	{
		if (m_verboseLogging)
			std::cout << "Unknown param name \"" << paramName << "\"\n";
		return;
	}

	auto cachedIt = m_shaderValueCache.find(paramName);
	if (cachedIt == m_shaderValueCache.end() || std::get<glm::mat4>(cachedIt->second) != value)
	{
		m_shaderValueCache[paramName] = value;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}