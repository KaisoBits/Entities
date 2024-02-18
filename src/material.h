#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "shaderprogram.h"
#include "texture.h"
#include "sun.h"

class Material
{
public:
	Material() = delete;
	Material(ShaderProgram shader) : m_shader(shader) { InitializeStandardUniforms(); }

	void AddTexture(const Texture& texture);
	void SetFloat(const std::string& paramName, float value);
	void SetVec4(const std::string& paramName, glm::vec4 value);

	void Use(const Sun& sun) const;

	unsigned int GetShaderId() const { return m_shader.GetId(); }

	void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
	[[nodiscard]] glm::vec3 GetAmbient() const { return m_ambient; }

	void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
	[[nodiscard]] glm::vec3 GetDiffuse() const { return m_diffuse; }

	void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
	[[nodiscard]] glm::vec3 GetSpecular() const { return m_specular; }

	void SetShininess(float shininess) { m_shininess = shininess; }
	[[nodiscard]] float GetShininess() const { return m_shininess; }

private:
	void InitializeStandardUniforms();
	void ApplyUniforms() const;
	void ApplyTextures() const;
	void ApplySun(const Sun& sun) const;

	std::vector<Texture> m_textures;
	std::map<int, float> m_floatUniforms;
	std::map<int, glm::vec4> m_vec4Uniforms;
	glm::vec3 m_ambient{ 1.f, 1.f, 1.f };
	int m_ambientLocation = -1;
	glm::vec3 m_diffuse{ 1.f, 1.f, 1.f };
	int m_diffuseLocation = -1;
	glm::vec3 m_specular{ 1.f, 1.f, 1.f };
	int m_specularLocation = -1;
	float m_shininess{ 32 };
	int m_shininessLocation = -1;

	int m_sunDirectionLocation = -1;
	int m_sunAmbientLocation = -1;
	int m_sunDiffuseLocation = -1;
	int m_sunSpecularLocation = -1;

	ShaderProgram m_shader;
};