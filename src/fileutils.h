#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

inline std::string readFileAsString(const std::string& path) {
	const std::ifstream stream(path);
	if (!stream.is_open()) {
		std::cout << "Could not open file " << path << '\n';
		return "";
	}
	std::stringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

inline std::vector<std::string> readFileAsLines(const std::string& path) {
	std::vector<std::string> result;
	
	std::ifstream stream(path);
	if (!stream.is_open()) {
		std::cout << "Could not open file " << path << '\n';
		return result;
	}
	std::string line;
	while (std::getline(stream, line)) {
		result.push_back(line);
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
