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
	m_diffuseMapLocation = m_shader.GetPramLocation("material.diffuseMap");
	m_diffuseOverrideLocation = m_shader.GetPramLocation("material.diffuseOverride");
	m_specularMapLocation = m_shader.GetPramLocation("material.specularMap");
	m_specularOverrideLocation = m_shader.GetPramLocation("material.specularOverride");
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

	int pos = 0;
	if (m_diffuseMapLocation >= 0 && m_diffuseMap)
	{
		glUniform1i(
			m_diffuseMapLocation,
			m_textures.size() + (pos++) /* diffuse gets appended at the end of the texture list*/);
		if (m_diffuseOverrideLocation >= 0)
			glUniform1i(m_diffuseOverrideLocation, GL_FALSE);
	}
	else if (m_diffuseOverrideLocation >= 0)
	{
		glUniform1i(m_diffuseOverrideLocation, GL_TRUE);
	}

	if (m_specularMapLocation >= 0 && m_specularMap)
	{
		glUniform1i(
			m_specularMapLocation,
			m_textures.size() + (pos++) /* specular gets appended at the end of the texture list*/);
		if (m_specularOverrideLocation >= 0)
			glUniform1i(m_specularOverrideLocation, GL_FALSE);
	}
	else if (m_specularOverrideLocation >= 0)
	{
		glUniform1i(m_specularOverrideLocation, GL_TRUE);
	}

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

	if (m_diffuseMap)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		m_diffuseMap->Use();
	}

	if (m_specularMap)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		m_specularMap->Use();
	}
}

void Material::Use(const Sun& sun) const
{
	m_shader.Use();
	ApplyUniforms();
	ApplyTextures();
	ApplySun(sun);
}