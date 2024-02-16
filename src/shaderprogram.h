#pragma once

#include <string>

class ShaderProgram
{
public:
	ShaderProgram() = delete;

	static ShaderProgram Compile(std::string vertexShaderSource, std::string fragmentShaderSource);

	int GetPramLocation(const std::string& paramName);
	void Use() const;

	unsigned int GetId() const { return m_programId; }

private:
	ShaderProgram(unsigned int programId) : m_programId(programId) {}

	unsigned int m_programId = 0;
};