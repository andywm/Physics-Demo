#pragma once
#include<wrl.h>
#include<d3d11.h>
#include<string>
#include "sys_shader.h"

namespace Render
{
	namespace d3d11
	{
		class Shader
		{
		public:
			
		private:
			Microsoft::WRL::ComPtr<ID3DBlob> mByteCode;
			Assets::SysShader::IDENTITY mType;

			Microsoft::WRL::ComPtr<ID3D11VertexShader> mVShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> mFShader;
			void allocResource(ID3D11Device * const devcon);

		public:	
			Shader(const std::string & sourcefile, 
				ID3D11Device * const dev,
				const Assets::SysShader::IDENTITY is);
			void activate(ID3D11DeviceContext * const devcon) const;
			inline ID3DBlob * blob() const
			{
				return mByteCode.Get();
			}
		};
	}
}