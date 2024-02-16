#include <string>
#include <vector>

#include "objParser.h"
#include "fileutils.h"

Model ObjParser::LoadFromFile(const std::string& filePath)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> endVertices;

	std::vector<glm::vec2> uv;
	std::vector<glm::vec2> endUv;

	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> endNormal;

	std::cout << "Loading model: " << filePath << '\n';
	std::streamoff fileSize = getFileSize(filePath);
	std::cout << "  Size: " << static_cast<double>(fileSize) / 1024.0 << "KiB\n";
	
	std::vector<std::string> lines = readFileAsLines(filePath);
	std::cout << "  Loaded data from file\n";


	unsigned long long read = 0;
	unsigned long long lineIndex = -1;
	for (const std::string& line : lines)
	{
		lineIndex++;
		
		read += line.size();

		if (line.starts_with('#'))
			continue;

		const std::vector tokens = TokenizeString(line, ' ');
		if (tokens.empty())
			continue;

		if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
			{
				std::cout << "Invalid data read at line {" << line << "} when parsing " << filePath << '\n';
				continue;
			}

			vertices.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "vn")
		{
			if (tokens.size() != 4)
			{
				std::cout << "Invalid data read at line {" << line << "} when parsing " << filePath << '\n';
				continue;
			}

			normal.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		}
		else if (tokens[0] == "vt")
		{
			if (tokens.size() != 3)
			{
				std::cout << "Invalid data read at line {" << line << "} when parsing " << filePath << '\n';
				continue;
			}

			uv.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]));
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::cout << "Invalid data read at line {" << line << "} when parsing " << filePath << '\n';
				continue;
			}

			auto vec = TokenizeString(tokens[1], '/');
			endVertices.push_back(vertices[std::stoi(vec[0]) - 1]);
			endUv.push_back(uv[std::stoi(vec[1]) - 1]);
			endNormal.push_back(normal[std::stoi(vec[2]) - 1]);

			vec = TokenizeString(tokens[2], '/');
			endVertices.push_back(vertices[std::stoi(vec[0]) - 1]);
			endUv.push_back(uv[std::stoi(vec[1]) - 1]);
			endNormal.push_back(normal[std::stoi(vec[2]) - 1]);

			vec = TokenizeString(tokens[3], '/');
			endVertices.push_back(vertices[std::stoi(vec[0]) - 1]);
			endUv.push_back(uv[std::stoi(vec[1]) - 1]);
			endNormal.push_back(normal[std::stoi(vec[2]) - 1]);
		}

		if (lineIndex % 30 == 0) // Update every x lines
			std::cout << "\r  Loaded: " << (static_cast<double>(read) / static_cast<double>(fileSize)) * 100 << "%       ";
	}

	std::cout << "\r  Loaded: 100.00%     \n";

	std::cout << "  Model loaded: " << filePath << '\n';

	return Model::Create(FlattenVector3(endVertices), FlattenVector2(endUv), FlattenVector3(endNormal));
}

std::vector<float> ObjParser::FlattenVector3(const std::vector<glm::vec3>& vector)
{
	std::vector<float> result(vector.size() * 3);
	int index = 0;

	for (const glm::vec3& v : vector)
	{
		result[index] = v.x;
		result[index + 1] = v.y;
		result[index + 2] = v.z;
		index += 3;
	}

	return result;
}

std::vector<float> ObjParser::FlattenVector2(const std::vector<glm::vec2>& vector)
{
	std::vector<float> result(vector.size() * 2);
	int index = 0;

	for (const glm::vec2& v : vector)
	{
		result[index] = v.x;
		result[index + 1] = v.y;
		index += 2;
	}

	return result;
}

std::vector<std::string> ObjParser::TokenizeString(std::string_view stringToTokenize, char separator)
{
	std::vector<std::string> result;

	bool isInChar = true;
	std::string current;
	
	for (const char c : stringToTokenize)
	{
		if (c == '#')
			break;

		if (isInChar && c == separator)
			continue;

		if (c != separator)
		{
			isInChar = false;
			current.push_back(c);
			continue;
		}

		if (!isInChar && c == separator)
		{
			isInChar = true;
			result.push_back(current);
			current.clear();
		}
	}

	if (!isInChar)
		result.push_back(current);

	return result;
}