#pragma once

#include <functional>
#include <optional>

#include "material.h"
#include "model.h"
#include "camera.h"

class Entity
{
public:
	explicit Entity(const Model* model, Material material) :
		m_model(model), m_material(material) {};

	void Draw(const Camera& camera) const;

	Material GetMaterial() const { return m_material; }
	void SetMaterial(Material newMaterial) { m_material = newMaterial; }

	const Model* GetModel() const { return m_model; }
	void SwitchModel(Model* newModel) { m_model = newModel; }

	void SetPosition(glm::vec3 position) { m_position = position; }
	glm::vec3 GetPosition() const { return m_position; }

	void SetRotation(glm::vec3 rotation) { m_rotation = rotation; }
	glm::vec3 GetRotation() const { return m_rotation; }

	void SetScale(glm::vec3 scale) { m_scale = scale; }
	glm::vec3 GetScale() const { return m_scale; }

	void Update(float deltaTime);

	void SetUpdateFunc(
		std::function<void(Entity* entity, const float deltaTime)> updateFunc);

private:
	void ApplyPositionAndRotation() const;

	const Model* m_model;
	Material m_material;

	glm::vec3 m_position{};
	glm::vec3 m_rotation{};
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	std::optional<std::function<void(Entity* entity, float deltaTime)>> m_updateFunc;
};