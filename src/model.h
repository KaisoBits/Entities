#pragma once

#include <vector>

class Model
{
public:
	Model() = delete;
	~Model();

	Model(Model&& other) noexcept;
	Model& operator= (Model&& other) noexcept;

	static Model Create(
		const std::vector<float>& vertices,
		const std::vector<float>& uvs,
		const std::vector<float>& normals);

	void Draw() const;

private:
	Model(unsigned int buffer, unsigned long long verticesCount) : 
		m_buffer(buffer), m_verticesCount(verticesCount) {}

	unsigned int m_buffer = 0;
	unsigned long long m_verticesCount = 0;

	static unsigned int s_currentlyBoundBuffer;
};