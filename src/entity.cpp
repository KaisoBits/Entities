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

	ApplyPositionAndRotation(m_material.GetShader());
	camera.Apply(m_material.GetShader());

	if (m_highlighted)
	{
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		m_model->Draw();

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glDepthRange(0, 0);

		m_material.UseHighlight();
		ApplyPositionAndRotation(m_material.GetHighlightShader(), 1.1f);
		camera.Apply(m_material.GetHighlightShader());
		m_model->Draw();
		glDepthRange(0, 1);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
	}
	else
	{
		m_model->Draw();
	}
}

void Entity::ApplyPositionAndRotation(ShaderProgram& shader, float scaleMult) const
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
			m_scale * scaleMult);

	shader.SetMat4("model", modelMatrix);
}

void Entity::Update(float deltaTime)
{
	if (m_shouldUpdate && m_updateFunc)
	{
		(*m_updateFunc)(this, deltaTime);
	}
}

void Entity::SetUpdateFunc(std::function<void(Entity* entity, float deltaTime)> updateFunc)
{
	m_updateFunc = std::move(updateFunc);
}