#pragma once
#include <string>

namespace Assets
{
	class SysShader
	{
	public:
		enum class IDENTITY {VS, FS};
	private:
		const std::string mFilePath;
		const std::string mName;
		IDENTITY mIdentity;
	public:
		SysShader(const std::string & file, 
			const std::string & sname,
			const SysShader::IDENTITY id);
		SysShader& operator=(const SysShader&) = delete;
		~SysShader() = default;
		inline const std::string &
		SysShader::name() const
		{
			return mName;
		}

		inline const std::string &
		SysShader::path() const
		{
			return mFilePath;
		}

		inline SysShader::IDENTITY
		SysShader::identity() const
		{
			return mIdentity;
		}
	};
}