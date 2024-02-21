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

void Material::ApplyPointLight(const PointLight& pointLight) const
{
	if (m_pointLightPositionLocation >= 0)
		glUniform3fv(m_pointLightPositionLocation, 1, &pointLight.position[0]);

	if (m_pointLightDiffuseLocation >= 0)
		glUniform3fv(m_pointLightDiffuseLocation, 1, &pointLight.diffuse[0]);

	if (m_pointLightSpecularLocation >= 0)
		glUniform3fv(m_pointLightSpecularLocation, 1, &pointLight.specular[0]);

	if (m_pointLightConstantLocation >= 0)
		glUniform1f(m_pointLightConstantLocation, pointLight.constant);

	if (m_pointLightLinearLocation >= 0)
		glUniform1f(m_pointLightLinearLocation, pointLight.linear);

	if (m_pointLightQuadraticLocation >= 0)
		glUniform1f(m_pointLightQuadraticLocation, pointLight.quadratic);
}

void Material::ApplySpotLight(const SpotLight& spotLight) const
{
	if (m_spotLightPositionLocation >= 0)
		glUniform3fv(m_spotLightPositionLocation, 1, &spotLight.position[0]);

	if (m_spotLightDirectionLocation >= 0)
		glUniform3fv(m_spotLightDirectionLocation, 1, &spotLight.direction[0]);

	if (m_spotLightDiffuseLocation >= 0)
		glUniform3fv(m_spotLightDiffuseLocation, 1, &spotLight.diffuse[0]);

	if (m_spotLightSpecularLocation >= 0)
		glUniform3fv(m_spotLightSpecularLocation, 1, &spotLight.specular[0]);

	if (m_spotLightConstantLocation >= 0)
		glUniform1f(m_spotLightConstantLocation, spotLight.constant);

	if (m_spotLightLinearLocation >= 0)
		glUniform1f(m_spotLightLinearLocation, spotLight.linear);

	if (m_spotLightQuadraticLocation >= 0)
		glUniform1f(m_spotLightQuadraticLocation, spotLight.quadratic);

	if (m_spotLightCutoffLocation >= 0)
		glUniform1f(m_spotLightCutoffLocation, spotLight.cutOff);
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

	m_sunDirectionLocation = m_shader.GetPramLocation("sun.direction");
	m_sunAmbientLocation = m_shader.GetPramLocation("sun.ambient");
	m_sunDiffuseLocation = m_shader.GetPramLocation("sun.diffuse");
	m_sunSpecularLocation = m_shader.GetPramLocation("sun.specular");

	m_pointLightPositionLocation = m_shader.GetPramLocation("pointLight.position");
	m_pointLightDiffuseLocation = m_shader.GetPramLocation("pointLight.diffuse");
	m_pointLightSpecularLocation = m_shader.GetPramLocation("pointLight.specular");
	m_pointLightConstantLocation = m_shader.GetPramLocation("pointLight.constant");
	m_pointLightLinearLocation = m_shader.GetPramLocation("pointLight.linear");
	m_pointLightQuadraticLocation = m_shader.GetPramLocation("pointLight.quadratic");

	m_spotLightPositionLocation = m_shader.GetPramLocation("spotLight.position");
	m_spotLightDirectionLocation = m_shader.GetPramLocation("spotLight.direction");
	m_spotLightDiffuseLocation = m_shader.GetPramLocation("spotLight.diffuse");
	m_spotLightSpecularLocation = m_shader.GetPramLocation("spotLight.specular");
	m_spotLightConstantLocation = m_shader.GetPramLocation("spotLight.constant");
	m_spotLightLinearLocation = m_shader.GetPramLocation("spotLight.linear");
	m_spotLightQuadraticLocation = m_shader.GetPramLocation("spotLight.quadratic");
	m_spotLightCutoffLocation = m_shader.GetPramLocation("spotLight.cutoff");
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

void Material::Use(const Sun& sun, const PointLight& pointLight, const SpotLight& spotLight) const
{
	m_shader.Use();
	ApplyUniforms();
	ApplyTextures();

	ApplySun(sun);
	ApplyPointLight(pointLight);
	ApplySpotLight(spotLight);
}