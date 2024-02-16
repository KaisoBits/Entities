#include <iostream>

#include <glad/glad.h>

#include "material.h"

void Material::AddTexture(const Texture& texture)
{
	int index = static_cast<int>(m_textures.size());
	std::string uniformName = "texture" + std::to_string(index);
	m_textures.push_back(texture);

	int location = m_shader.GetPramLocation(uniformName);
	if (location >= 0)
	{
		m_shader.Use();
		glUniform1i(location, index);
	}
	else
	{
		std::cout << "Attempted to add texture number " << index << " to material with shader with not enough samplers.\n";
	}
}

void Material::SetFloat(const std::string& paramName, float value)
{
	int location = m_shader.GetPramLocation(paramName);

	if (location >= 0)
		m_floatUniforms[location] = value;
}

void Material::SetVec4(const std::string& paramName, glm::vec4 value)
{
	int location = m_shader.GetPramLocation(paramName);

	if (location >= 0)
		m_vec4Uniforms[location] = value;
}

void Material::ApplyUniforms() const
{
	for (const auto& [location, value] : m_floatUniforms)
		glUniform1f(location, value);

	for (const auto& [location, value] : m_vec4Uniforms)
		glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Material::ApplyTextures() const
{
	int n = 0;
	for (const auto& texture : m_textures)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		texture.Use();
		n++;
	}
}

void Material::Use() const
{
	m_shader.Use();
	ApplyUniforms();
	ApplyTextures();
}