#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "shaderprogram.h"
#include "texture.h"
#include "sun.h"
#include "pointlight.h"
#include "spotlight.h"

class Material
{
public:
	Material() = delete;
	Material(ShaderProgram shader) : m_shader(shader) { InitializeStandardUniforms(); }

	void SetFloat(const std::string& paramName, float value);
	void SetVec4(const std::string& paramName, glm::vec4 value);

	void Use(const Sun& sun, const PointLight& pointLight, const SpotLight& spotLight) const;

	unsigned int GetShaderId() const { return m_shader.GetId(); }

	void SetColor(glm::vec3 color) { m_color = color; }
	[[nodiscard]] glm::vec3 GetColor() const { return m_color; }

	void SetDiffuseMap(const Texture* diffuseMap) { m_diffuseMap = diffuseMap; }
	[[nodiscard]] const Texture* GetDiffuseMap() const { return m_diffuseMap; }

	void SetSpecularMap(const Texture* specularMap) { m_specularMap = specularMap; }
	[[nodiscard]] const Texture* GetSpecularMap() const { return m_specularMap; }

	void SetShininess(float shininess) { m_shininess = shininess; }
	[[nodiscard]] float GetShininess() const { return m_shininess; }

private:
	void InitializeStandardUniforms();
	void ApplyUniforms() const;
	void ApplyTextures() const;
	void ApplySun(const Sun& sun) const;
	void ApplyPointLight(const PointLight& pointLight) const;
	void ApplySpotLight(const SpotLight& spotLight) const;

	std::map<int, float> m_floatUniforms;
	std::map<int, glm::vec4> m_vec4Uniforms;

	glm::vec3 m_color{ 1.f, 1.f, 1.f };
	int m_colorLocation = -1;

	const Texture* m_diffuseMap = nullptr;
	int m_diffuseMapLocation = -1;
	int m_diffuseOverrideLocation = -1;

	const Texture* m_specularMap = nullptr;
	int m_specularMapLocation = -1;
	int m_specularOverrideLocation = -1;

	float m_shininess{ 32 };
	int m_shininessLocation = -1;

	int m_sunDirectionLocation = -1;
	int m_sunAmbientLocation = -1;
	int m_sunDiffuseLocation = -1;
	int m_sunSpecularLocation = -1;

	int m_pointLightPositionLocation = -1;
	int m_pointLightDiffuseLocation = -1;
	int m_pointLightSpecularLocation = -1;
	int m_pointLightConstantLocation = -1;
	int m_pointLightLinearLocation = -1;
	int m_pointLightQuadraticLocation = -1;

	int m_spotLightPositionLocation = -1;
	int m_spotLightDirectionLocation = -1;
	int m_spotLightDiffuseLocation = -1;
	int m_spotLightSpecularLocation = -1;
	int m_spotLightConstantLocation = -1;
	int m_spotLightLinearLocation = -1;
	int m_spotLightQuadraticLocation = -1;
	int m_spotLightCutoffLocation = -1;

	ShaderProgram m_shader;
};