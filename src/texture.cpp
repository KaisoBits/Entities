#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glad/glad.h>

#include "texture.h"

Texture Texture::LoadFromFile(const std::string& path)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channelsCount;
	unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &channelsCount, 0);

	if (!imageData)
	{
		std::cout << "Failed to load texture " << path << "\n";
		return Empty();
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	return Texture(texture);
}

void Texture::Use() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}