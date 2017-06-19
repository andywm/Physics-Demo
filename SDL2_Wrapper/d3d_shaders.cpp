#pragma once
#include <d3dcompiler.h>
#include<wrl.h>
#include<d3d11.h>
#include<string>
#include "d3d_shaders.h"
#include "sys_shader.h"

#pragma comment(lib, "d3dcompiler.lib") //move this...
/*
-------------------- Definition : D3DShader ------------------------------------
*/

namespace Render
{
	namespace d3d11
	{
		Shader::Shader(const std::string & sourcefile, 
			ID3D11Device * const dev,
			const Assets::SysShader::IDENTITY is)
			:
			mType(is)
		{
			const UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
			
			std::string shaderTarget;
			switch (is)
			{
			case Assets::SysShader::IDENTITY::VS:
				shaderTarget = "vs_5_0";
				break;
			case Assets::SysShader::IDENTITY::FS:
				shaderTarget = "ps_5_0";
				break;
			}

			//.GetAddressOf()
			Microsoft::WRL::ComPtr<ID3DBlob> code_errors;
			std::wstring temp;
			for (const auto c : sourcefile)
				temp += c;
			const LPCWSTR file = temp.c_str();

			const HRESULT hr =
			D3DCompileFromFile(
				file,
				nullptr,
				nullptr,
				"main",
				shaderTarget.c_str(),
				flags,
				0,
				&mByteCode,
				&code_errors

			);
			
			if (hr == S_OK)
			{
				allocResource(dev);
			}
		}
		void Shader::activate(ID3D11DeviceContext * const devcon) const
		{
			switch (mType)
			{
			case Assets::SysShader::IDENTITY::VS:
				devcon->VSSetShader(mVShader.Get(), nullptr, 0);
				break;
			case Assets::SysShader::IDENTITY::FS:
				devcon->PSSetShader(mFShader.Get(), nullptr, 0);
				break;
			}
		}

		void Shader::allocResource(ID3D11Device * const dev)
		{
			switch (mType)
			{
			case Assets::SysShader::IDENTITY::VS:
				dev->CreateVertexShader(mByteCode->GetBufferPointer(),
					mByteCode->GetBufferSize(), nullptr, &mVShader);
				break;
			case Assets::SysShader::IDENTITY::FS:
				dev->CreatePixelShader(mByteCode->GetBufferPointer(),
					mByteCode->GetBufferSize(), nullptr, &mFShader);
				break;
			}
		}
	}
}
