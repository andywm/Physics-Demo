#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <glm\glm.hpp>

namespace AssociativeLanguageParser
{
	namespace Extended
	{
		const glm::vec3
		vector3FromString(const std::string & str)
		{
			float v1, v2, v3; char discard;
			std::istringstream iss(str, std::ios::in);
			iss >> v1 >> discard >> v2 >> discard >> v3;

			return glm::vec3(v1, v2, v3);
		}

		const glm::vec4
		vector4FromString(const std::string & str)
		{
			float v1, v2, v3, v4; char discard;
			std::istringstream iss(str, std::ios::in);
			iss >> v1 >> discard >> v2 >> discard >> v3 >> discard >> v4;

			return glm::vec4(v1, v2, v3, v4);
		}

	}

	std::vector<std::string>
	parse(std::string & target)
	{
		std::vector<std::string> elements;
		std::istringstream iss(target, std::istringstream::in);
		std::string result = "";

		while (std::getline(iss, result, ':'))
		{
			elements.push_back(result);
			result = "";
		}
		return elements;
	}

	bool
	readFile(const std::string & file,
		std::vector<std::vector<std::string>> & store)
	{
		store.clear();
		std::ifstream ifs(file, std::ios::in);
		if (ifs.is_open())
		{
			std::string line;
			while(getline(ifs, line))
			{
				if (line != "") store.push_back(parse(line));
				line = "";
			}
		}
		return (bool)store.size;
	}
}