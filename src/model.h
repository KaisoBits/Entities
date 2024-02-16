#pragma once

#include <vector>

class Model
{
public:
	Model() = delete;

	static Model Create(
		const std::vector<float>& vertices,
		const std::vector<float>& uvs,
		const std::vector<float>& normals);

	void Draw() const;

private:
	Model(unsigned int buffer, unsigned long long verticesCount) : 
		m_buffer(buffer), m_verticesCount(verticesCount) {}

	unsigned int m_buffer;
	unsigned long long m_verticesCount;
};