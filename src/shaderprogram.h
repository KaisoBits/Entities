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

	~ShaderProgram();
	ShaderProgram(ShaderProgram&& other) noexcept;
	ShaderProgram& operator=(ShaderProgram&& other) noexcept;

	[[nodiscard]] int GetPramLocation(const std::string& paramName) const;
	void Use() const;

	[[nodiscard]] unsigned int GetId() const { return m_programId; }

	void SetInt(const std::string& paramName, int value);
	void SetUint(const std::string& paramName, unsigned int value);
	void SetFloat(const std::string& paramName, float value);
	void SetVector3(const std::string& paramName, const glm::vec3& value);
	void SetVector2(const std::string& paramName, const glm::vec2& value);
	void SetMat4(const std::string& paramName, const glm::mat4& value);
	void SetVerboseLogging(bool verboseLogging) { m_verboseLogging = verboseLogging; }

	static unsigned int GetCurrentShader() { return s_currentlyUsedShader; }

private:
	explicit ShaderProgram(unsigned int programId) : m_programId(programId) {}

	unsigned int m_programId = 0;
	bool m_verboseLogging = false;

	mutable std::unordered_map<std::string, int> m_shaderLocationCache;
	using ShaderValue = std::variant<glm::vec3, glm::vec2, glm::mat4, int, unsigned int, float>;
	mutable std::unordered_map<std::string, ShaderValue> m_shaderValueCache;

	static unsigned int s_currentlyUsedShader;
};