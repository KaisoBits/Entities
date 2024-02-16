#pragma once

#include <string>

class ShaderProgram
{
public:
	static ShaderProgram Compile(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	static ShaderProgram Empty() { return ShaderProgram(0); }

	[[nodiscard]] int GetPramLocation(const std::string& paramName) const;
	void Use() const;

	[[nodiscard]] unsigned int GetId() const { return m_programId; }

private:
	explicit ShaderProgram(unsigned int programId) : m_programId(programId) {}

	unsigned int m_programId = 0;
};