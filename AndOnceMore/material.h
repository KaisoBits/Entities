#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "shaderprogram.h"
#include "texture.h"

class Material
{
public:
	Material() = delete;
	Material(ShaderProgram shader) : m_shader(shader) { }

	void AddTexture(const Texture& texture);
	void SetFloat(const std::string& paramName, float value);
	void SetVec4(const std::string& paramName, glm::vec4 value);

	void Use() const;

	unsigned int GetShaderId() const { return m_shader.GetId(); }

private:
	void ApplyUniforms() const;
	void ApplyTextures() const;

	std::vector<Texture> m_textures;
	std::map<int, float> m_floatUniforms;
	std::map<int, glm::vec4> m_vec4Uniforms;
	ShaderProgram m_shader;
};