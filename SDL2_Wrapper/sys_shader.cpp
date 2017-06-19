#include <string>
#include "sys_shader.h"

namespace Assets
{
	SysShader::SysShader(const std::string & file,
						 const std::string & sname,
						 const SysShader::IDENTITY id)
		:
		mFilePath(file),
		mIdentity(id),
		mName(sname)
	{}


}