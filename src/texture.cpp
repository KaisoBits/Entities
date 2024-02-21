#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glad/glad.h>

#include "texture.h"

Texture Texture::LoadFromFile(const std::string& path)
{
	std::cout << "Loading texture " << path << "\n";
	stbi_set_flip_vertically_on_load(true);

	int width, height, channelsCount;
	unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &channelsCount, 0);

	if (!imageData)
	{
		std::cout << "  Failed to load texture " << path << "\n";
		return Empty();
	}

	std::cout << "  Loaded from file\n";

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (channelsCount == 1)
	{
		// Grayscale to rgb
		size_t dataSize = width * height;
		auto rgbImageData = new unsigned char[dataSize * 3];

		for (size_t i = 0; i < dataSize; ++i)
		{
			rgbImageData[i * 3 + 0] = imageData[i]; // R
			rgbImageData[i * 3 + 1] = imageData[i]; // G
			rgbImageData[i * 3 + 2] = imageData[i]; // B
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbImageData);

		delete[] rgbImageData;
	}
	else if (channelsCount == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "  Sent to GPU\n";

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	return Texture(texture);
}

void Texture::Use() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}