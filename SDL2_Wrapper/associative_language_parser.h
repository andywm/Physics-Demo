#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <glm\glm.hpp>

namespace AssociativeLanguageParser
{
	std::vector<std::string> parse(std::string & target);
	bool readFile(
		const std::string & file,std::vector<std::vector<std::string>> & store); 

	namespace Extended
	{
		const glm::vec3 vector3FromString(const std::string & str);
		const glm::vec4 vector4FromString(const std::string & str);
	}

}