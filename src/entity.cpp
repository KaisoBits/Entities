#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entity.h"

void Entity::Draw(const Camera& camera,
	const std::vector<Sun>& suns,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights) const
{
	if (!m_model)
		return;

	m_material.Use(suns, pointLights, spotLights);

	ApplyPositionAndRotation();
	camera.Apply(m_material.GetShader());

	m_model->Draw();
}

void Entity::ApplyPositionAndRotation() const
{
	glm::mat4 modelMatrix =
		glm::scale(
			glm::rotate(
				glm::rotate(
					glm::rotate(
						glm::translate(glm::mat4(1.0f), m_position),
						m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)),
					m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)
				),
				m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)),
			m_scale);

	ShaderProgram& shader = m_material.GetShader();

	shader.SetMat4("model", modelMatrix);
}

void Entity::Update(float deltaTime)
{
	if (m_updateFunc)
	{
		(*m_updateFunc)(this, deltaTime);
	}
}

void Entity::SetUpdateFunc(std::function<void(Entity* entity, float deltaTime)> updateFunc)
{
	m_updateFunc = std::move(updateFunc);
}