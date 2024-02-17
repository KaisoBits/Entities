#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entity.h"

void Entity::Draw(const Camera& camera) const
{
	if (!m_model)
		return;

	m_material.Use();

	ApplyPositionAndRotation();
	camera.Apply(m_material.GetShaderId());

	m_model->Draw();
}

void Entity::ApplyPositionAndRotation() const
{
	int modelMatrixLocation = glGetUniformLocation(m_material.GetShaderId(), "model");
	if (modelMatrixLocation >= 0)
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

		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	}
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