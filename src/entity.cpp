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
	ApplyCamera(m_material.GetShader(), camera);

	if (m_highlighted)
	{
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		m_model->Draw();

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glDepthRange(0, 0);

		m_material.UseHighlight();
		ApplyPositionAndRotation(m_material.GetHighlightShader(), .2f);
		ApplyCamera(m_material.GetHighlightShader(), camera);
		m_model->Draw();
		glDepthRange(0, 1);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
	}
	else
	{
		m_model->Draw();
	}
}

void Entity::ApplyPositionAndRotation(ShaderProgram& shader, float scaleIncrease) const
{
	glm::mat4 modelMatrix =
		glm::scale(
			glm::rotate(
				glm::rotate(
					glm::rotate(
						glm::translate(glm::mat4(1.0f), m_position),
						glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)),
					glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)
				),
				glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)),
			m_scale + glm::vec3(scaleIncrease));

	shader.SetMat4("model", modelMatrix);
}

void Entity::ApplyCamera(ShaderProgram& shader, const Camera& camera) const
{
	shader.SetMat4("view", camera.GetMatrix());

	glm::mat4 perspective =
		glm::perspective(glm::radians(camera.GetFovY()), camera.GetAspectRatio(), 0.1f, 1000.0f);
	shader.SetMat4("perspective", perspective);
	shader.SetVector3("cameraPosition", camera.GetPosition());
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