#pragma once
#include<glm\mat4x4.hpp>
#include<d3d11.h>
#include<wrl.h>
#include "d3d_context.h"
#include "mesh.h"
#include <cstddef>
#include <array>
#include "d3d_shaders.h"

namespace Render
{
	namespace d3d11
	{
		class D3DContext;
		class Geometry
		{
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> mVBO;
			Microsoft::WRL::ComPtr<ID3D11Buffer> mIBO; 		
			Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
			unsigned mIndexCount;
			D3D11_PRIMITIVE_TOPOLOGY mTopology;


			

			const std::array<const D3D11_INPUT_ELEMENT_DESC, 3> cmLayoutPreset={
			{
				{ "POSITION",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0
				},

				{ "NORMAL",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					sizeof(float)*3,
					D3D11_INPUT_PER_VERTEX_DATA,
					0
				},

				{ "TEXCOORD",
					0,
					DXGI_FORMAT_R32G32_FLOAT,
					0,
					sizeof(float) * 6,
					D3D11_INPUT_PER_VERTEX_DATA,
					0
				}
			}};

		public:
			Geometry(D3DContext & context, const Assets::Mesh & mesh);
			Geometry& operator=(const Geometry&) = delete;
			void setInputLayout(D3DContext & context, const Shader & vs);
			void draw(D3DContext & context);
		};
	}
}
