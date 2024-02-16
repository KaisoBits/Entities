#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>

inline std::string readFileAsString(const std::string& path)
{
	std::string result;
	std::ifstream stream(path);

	if (!stream.is_open()) {
		std::cout << "Could not open file " << path << '\n';
		return "";
	}

	std::string line;
	while (std::getline(stream, line))
	{
		result.append(line);
		result.append("\n");
	}

	return result;
}

inline std::ifstream readFileAsStream(const std::string& path)
{
	std::ifstream stream(path);

	if (!stream.is_open()) {
		std::cout << "Could not open file " << path << '\n';
	}

	return stream;
}

inline std::streamoff getFileSize(const std::string& path)
{
	std::ifstream fileStream(path, std::ios::binary | std::ios::ate);

	return fileStream.tellg();
}
