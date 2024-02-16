#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include "glm/glm.hpp"
#include "model.h"

class ObjParser
{
public:
	static Model LoadFromFile(const std::string& filePath);

private:
	static std::vector<float> FlattenVector2(const std::vector<glm::vec2>& vector);
	static std::vector<float> FlattenVector3(const std::vector<glm::vec3>& vector);
	static std::vector<std::string> TokenizeString(std::string_view stringToTokenize, char separator);
};