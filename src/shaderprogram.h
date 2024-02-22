#pragma once

#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>

class ShaderProgram
{
public:
	static ShaderProgram Compile(
		const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	static ShaderProgram Empty() { return ShaderProgram(0); }

	[[nodiscard]] int GetPramLocation(const std::string& paramName) const;
	void Use() const;

	[[nodiscard]] unsigned int GetId() const { return m_programId; }

	void SetInt(const std::string& paramName, int value) const;
	void SetFloat(const std::string& paramName, float value) const;
	void SetVector3(const std::string& paramName, glm::vec3 value) const;
	void SetVector2(const std::string& paramName, glm::vec2 value) const;

private:
	explicit ShaderProgram(unsigned int programId) : m_programId(programId) {}

	unsigned int m_programId = 0;
	mutable std::unordered_map<std::string, int> m_shaderLocationCache;
	mutable std::unordered_map<std::string, 
		std::variant<glm::vec3, glm::vec2, int, float>> m_shaderValueCache;
};