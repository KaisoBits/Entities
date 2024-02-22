#include <iostream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "material.h"

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

void Material::ApplyUniforms() const
{
	m_shader.SetVector3("material.color", m_color);
	m_shader.SetInt("material.diffuseMap", 0);
	m_shader.SetInt("material.diffuseOverride", m_diffuseMap == nullptr);
	m_shader.SetInt("material.specularMap", 1);
	m_shader.SetInt("material.specularOverride", m_specularMap == nullptr);
	m_shader.SetFloat("material.shininess", m_shininess);
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