#include <iostream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "material.h"

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

void Material::ApplySuns(const std::vector<Sun>& suns) const
{
	m_shader.SetInt("sunsCount", suns.size());
	for (int i = 0; i < suns.size(); i++)
	{
		const Sun& sun = suns[i];
		const std::string member = "suns[" + std::to_string(i) + "]";
		m_shader.SetVector3(member + ".direction", sun.direction);
		m_shader.SetVector3(member + ".ambient", sun.ambient);
		m_shader.SetVector3(member + ".diffuse", sun.diffuse);
		m_shader.SetVector3(member + ".specular", sun.specular);
	}
}

void Material::ApplyPointLights(const std::vector<PointLight>& pointLights) const
{
	m_shader.SetInt("pointLightsCount", pointLights.size());
	for (int i = 0; i < pointLights.size(); i++)
	{
		const PointLight& pointLight = pointLights[i];
		const std::string member = "pointLights[" + std::to_string(i) + "]";
		m_shader.SetVector3(member + ".position", pointLight.position);
		m_shader.SetVector3(member + ".diffuse", pointLight.diffuse);
		m_shader.SetVector3(member + ".specular", pointLight.specular);
		m_shader.SetFloat(member + ".constant", pointLight.constant);
		m_shader.SetFloat(member + ".linear", pointLight.linear);
		m_shader.SetFloat(member + ".quadratic", pointLight.quadratic);
	}
}

void Material::ApplySpotLights(const std::vector<SpotLight>& spotLights) const
{
	m_shader.SetInt("spotLightsCount", spotLights.size());
	for (int i = 0; i < spotLights.size(); i++)
	{
		const SpotLight& pointLight = spotLights[i];
		const std::string member = "spotLights[" + std::to_string(i) + "]";
		m_shader.SetVector3(member + ".position", pointLight.position);
		m_shader.SetVector3(member + ".direction", pointLight.direction);
		m_shader.SetVector3(member + ".diffuse", pointLight.diffuse);
		m_shader.SetVector3(member + ".specular", pointLight.specular);
		m_shader.SetFloat(member + ".constant", pointLight.constant);
		m_shader.SetFloat(member + ".linear", pointLight.linear);
		m_shader.SetFloat(member + ".quadratic", pointLight.quadratic);
		m_shader.SetFloat(member + ".innerCutoff", pointLight.innerCutoff);
		m_shader.SetFloat(member + ".outerCutoff", pointLight.outerCutoff);
	}
}

void Material::InitializeStandardUniforms()
{
	m_shader.Use();

	m_colorLocation = m_shader.GetPramLocation("material.color");
	m_diffuseMapLocation = m_shader.GetPramLocation("material.diffuseMap");
	if (m_diffuseMapLocation >= 0) glUniform1i(m_diffuseMapLocation, 0);
	m_diffuseOverrideLocation = m_shader.GetPramLocation("material.diffuseOverride");
	m_specularMapLocation = m_shader.GetPramLocation("material.specularMap");
	if (m_specularMapLocation >= 0) glUniform1i(m_specularMapLocation, 1);
	m_specularOverrideLocation = m_shader.GetPramLocation("material.specularOverride");
	m_shininessLocation = m_shader.GetPramLocation("material.shininess");
}

void Material::ApplyUniforms() const
{
	if (m_colorLocation >= 0)
		glUniform3fv(m_colorLocation, 1, &m_color[0]);

	if (m_diffuseOverrideLocation >= 0)
		glUniform1i(m_diffuseOverrideLocation, m_diffuseMap == nullptr);

	if (m_specularOverrideLocation >= 0)
		glUniform1i(m_specularOverrideLocation, m_specularMap == nullptr);

	if (m_shininessLocation >= 0)
		glUniform1f(m_shininessLocation, m_shininess);

	for (const auto& [location, value] : m_floatUniforms)
		glUniform1f(location, value);

	for (const auto& [location, value] : m_vec4Uniforms)
		glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Material::ApplyTextures() const
{
	if (m_diffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		m_diffuseMap->Use();
	}

	if (m_specularMap)
	{
		glActiveTexture(GL_TEXTURE1);
		m_specularMap->Use();
	}
}

void Material::Use(const std::vector<Sun>& suns,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights) const
{
	m_shader.Use();
	ApplyUniforms();
	ApplyTextures();

	ApplySuns(suns);
	ApplyPointLights(pointLights);
	ApplySpotLights(spotLights);
}