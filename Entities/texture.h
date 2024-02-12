#pragma once

#include <string>

class Texture
{
public:
	static Texture LoadFromFile(const std::string& path);

	Texture() = delete;

	void Use() const;

private:
	Texture(unsigned int texture) : m_texture(texture) {}

	unsigned int m_texture;
};