#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(float fovY, float aspectRatio) : m_fovY(fovY), m_aspectRatio(aspectRatio) {}

	glm::mat4 GetMatrix() const
	{
		glm::mat4 viewMatrix =
			glm::translate(
				glm::rotate(
					glm::rotate(
						glm::mat4(1.0f),
						glm::radians(m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f)
					),
					glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f)
				), -m_position);

		return viewMatrix;
	}

	glm::vec2 GetRotation() const { return m_rotation; }
	void SetRotation(glm::vec2 rotation) { m_rotation = rotation; }

	glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 position) { m_position = position; }

	float GetFovY() const { return m_fovY; }
	void SetFovY(float fovY) { m_fovY = fovY; }

	float GetAspectRatio() const { return m_aspectRatio; }
	void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }

	void Apply(int shaderId) const
	{
		int viewMatrixLocation = glGetUniformLocation(shaderId, "view");
		if (viewMatrixLocation >= 0)
		{
			glm::mat4 viewMatrix = GetMatrix();

			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		}

		int perspectiveMatrixLocation = glGetUniformLocation(shaderId, "perspective");
		if (perspectiveMatrixLocation >= 0)
		{
			glm::mat4 perspective =
				glm::perspective(glm::radians(m_fovY), m_aspectRatio, 0.1f, 1000.0f);

			glUniformMatrix4fv(perspectiveMatrixLocation, 1, GL_FALSE, glm::value_ptr(perspective));
		}
	}

private:
	glm::vec2 m_rotation = glm::vec2(0, 0);
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	float m_fovY;
	float m_aspectRatio;
};
