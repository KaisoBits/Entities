#include <iostream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

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

void Material::ApplySun(const Sun& sun) const
{
	if (m_sunDirectionLocation >= 0)
		glUniform3fv(m_sunDirectionLocation, 1, glm::value_ptr(sun.direction));

	if (m_sunAmbientLocation >= 0)
		glUniform3fv(m_sunAmbientLocation, 1, glm::value_ptr(sun.ambient));

	if (m_sunDiffuseLocation >= 0)
		glUniform3fv(m_sunDiffuseLocation, 1, glm::value_ptr(sun.diffuse));

	if (m_sunSpecularLocation >= 0)
		glUniform3fv(m_sunSpecularLocation, 1, glm::value_ptr(sun.specular));
}

void Material::InitializeStandardUniforms()
{
	m_ambientLocation = m_shader.GetPramLocation("material.ambient");
	m_diffuseLocation = m_shader.GetPramLocation("material.diffuse");
	m_specularLocation = m_shader.GetPramLocation("material.specular");
	m_shininessLocation = m_shader.GetPramLocation("material.shininess");

	m_sunDirectionLocation = m_shader.GetPramLocation("sun.direction");
	m_sunAmbientLocation = m_shader.GetPramLocation("sun.ambient");
	m_sunDiffuseLocation = m_shader.GetPramLocation("sun.diffuse");
	m_sunSpecularLocation = m_shader.GetPramLocation("sun.specular");
}

void Material::ApplyUniforms() const
{
	if (m_ambientLocation >= 0)
		glUniform3fv(m_ambientLocation, 1, &m_ambient[0]);

	if (m_diffuseLocation >= 0)
		glUniform3fv(m_diffuseLocation, 1, &m_diffuse[0]);

	if (m_specularLocation >= 0)
		glUniform3fv(m_specularLocation, 1, &m_specular[0]);

	if (m_shininessLocation >= 0)
		glUniform1f(m_shininessLocation, m_shininess);

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

void Material::Use(const Sun& sun) const
{
	m_shader.Use();
	ApplyUniforms();
	ApplyTextures();
	ApplySun(sun);
}