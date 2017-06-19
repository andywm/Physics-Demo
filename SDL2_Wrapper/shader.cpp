#include "d3d_context.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

namespace Render
{
	std::vector<std::string> shaders;
	bool
	D3DContext::Asset::loadGeometry(const int assetid, Mesh & mesh)
	{

	}
	bool
	D3DContext::Asset::loadShader(const std::string & source, 
		const std::string & name)
	{
		//HRESULT hr = D3DCompileFromFile
		//(
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob * ppCode;
		ID3DBlob * ppErrors;
		char*shaderByteCode;
		HRESULT hr = D3DCompile(
			(LPCVOID)source.c_str(),
			static_cast<SIZE_T>(source.size()),
			nullptr,
			nullptr,
			nullptr,
			nullptr, //this one is actually quite important
			shaderByteCode,
			flags,
			0,
			&ppCode,
			&ppErrors
		);
		//);
		
	}
}
