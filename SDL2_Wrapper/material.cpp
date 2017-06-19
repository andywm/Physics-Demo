#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include "associative_language_parser.h"
#include "material.h"

namespace Assets
{
	namespace APL = AssociativeLanguageParser;
	Material::Material(const std::string & path)
		:
		mSpecular(0),
		mDiffuse(0),
		mAmbient(0),
		mSpecularExponent(0)
	{
		std::vector<std::vector<std::string>> file;
		APL::readFile(path, file);
		for (const auto & line : file)
		{
			if (line[0] == "s")
			{
				mSpecular = APL::Extended::vector4FromString(line[1]);
			}
			else if (line[0] == "d")
			{
				mDiffuse = APL::Extended::vector4FromString(line[1]);
			}
			else if (line[0] == "a")
			{
				mAmbient = APL::Extended::vector4FromString(line[1]);
			}
			else if (line[0] == "e")
			{
				std::istringstream iss(line[1], std::ios::in);
				iss >> mSpecularExponent;
			}
		}
	}

	Material::Material(const glm::vec4 & diff,
		const glm::vec4 & amb,
		const glm::vec4 & spec,
		float exponent)
		:
		mSpecular(spec),
		mDiffuse(diff),
		mAmbient(amb),
		mSpecularExponent(exponent)
	{}
}