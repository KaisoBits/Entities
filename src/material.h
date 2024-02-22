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
	Material(ShaderProgram shader) : m_shader(shader) { }

	void Use(
		const std::vector<Sun>& sun,
		const std::vector<PointLight>& pointLight,
		const std::vector<SpotLight>& spotLight) const;

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
	void ApplyUniforms() const;
	void ApplyTextures() const;
	void ApplySuns(const std::vector<Sun>& suns) const;
	void ApplyPointLights(const std::vector<PointLight>& pointLight) const;
	void ApplySpotLights(const std::vector<SpotLight>& spotLight) const;

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

	ShaderProgram m_shader;
};