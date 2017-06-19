#pragma once
#include<glm\glm.hpp>
#include<glm\gtx\transform.hpp>
#include<d3d11.h>
#include<wrl.h>
#include <cstddef>
#include <array>
#include"d3d_gpu_geometry.h"
#include "d3d_context.h"
#include "mesh.h"
#include "d3d_shaders.h"

namespace Render
{
	//abilash.gaming
	//gaming@123
	//http://www.directxtutorial.com/lesson.aspx?lessonid=11-4-5
	//http://www.directxtutorial.com/LessonList.aspx?listid=11
	namespace d3d11
	{
		Geometry::Geometry(D3DContext & context, const Assets::Mesh & mesh)
			:
			mIndexCount(mesh.indiciesCount()),
			mTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			//describe VBO buffer
			const D3D11_BUFFER_DESC vboDesc
			{
				mesh.attributeByteSize()*mesh.verticesCount(),
				D3D11_USAGE_DYNAMIC,
				D3D11_BIND_VERTEX_BUFFER,
				D3D11_CPU_ACCESS_WRITE,
				0,0
			};

			//describe IBO Buffer
			const D3D11_BUFFER_DESC iboDesc
			{
				mesh.indexByteSize() * mesh.indiciesCount(),
				D3D11_USAGE_DYNAMIC,
				D3D11_BIND_INDEX_BUFFER,
				D3D11_CPU_ACCESS_WRITE,
				0,0
			};

			//create VBO and IBO buffers.
			const auto dev = context.device();
			HRESULT hr = dev->CreateBuffer(&vboDesc, nullptr, &mVBO);
			hr = dev->CreateBuffer(&iboDesc, nullptr, &mIBO);

			//populate those buffers.
			const auto con = context.deviceContext();

			D3D11_MAPPED_SUBRESOURCE vboData;
			con->Map(mVBO.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &vboData);
			memcpy(vboData.pData, 
				mesh.vertices(), 
				Assets::Mesh::attributeByteSize()*mesh.verticesCount());
			con->Unmap(mVBO.Get(), NULL);

			D3D11_MAPPED_SUBRESOURCE iboData;
			con->Map(mIBO.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &iboData);
			memcpy(iboData.pData,
				mesh.indicies(), 
				Assets::Mesh::indexByteSize() * mesh.indiciesCount());
			con->Unmap(mIBO.Get(), NULL);

			//constant buffers
			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; // bilinear
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.MipLODBias = 0.0f;
			sampDesc.MaxAnisotropy = 1;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampDesc.BorderColor[0] = 0;
			sampDesc.BorderColor[1] = 0;
			sampDesc.BorderColor[2] = 0;
			sampDesc.BorderColor[3] = 0;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
			

			hr = dev->CreateSamplerState(&sampDesc, &mSampler);

		}

		void Geometry::setInputLayout(D3DContext & context, const Shader & vs)
		{
			const auto dev = context.device();

			ID3DBlob * const blob = vs.blob();
			dev->CreateInputLayout(&cmLayoutPreset[0], cmLayoutPreset.size(),
				blob->GetBufferPointer(), blob->GetBufferSize(), &mInputLayout);
		}

		void Geometry::draw(D3DContext & context)
		{
			const auto con = context.deviceContext();

			//prepare input assembly
			con->IASetInputLayout(mInputLayout.Get());
			con->IASetPrimitiveTopology(mTopology);

			unsigned const stride(Assets::Mesh::attributeByteSize());
			unsigned const offset(0);

			con->IASetVertexBuffers(0,1, mVBO.GetAddressOf(), &stride, &offset);
			con->IASetIndexBuffer(mIBO.Get(), DXGI_FORMAT_R32_UINT, 0);

			//prepare vertex stage
			struct subresource
			{
				glm::mat4 model;
				glm::mat4 model_it;
			} 
			const data{ context.pipeline().model(),
				glm::transpose(glm::inverse(context.pipeline().model())) };

			
			ID3D11Buffer *pCBuffer;
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(glm::mat4) * 2;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			context.device()->CreateBuffer(&bd, nullptr, &pCBuffer);
			con->UpdateSubresource(pCBuffer, 0, nullptr, &data, 0, 0);
					
			con->VSSetConstantBuffers(1, 1, &pCBuffer);//use slot 1.
			con->PSSetConstantBuffers(1, 1, &pCBuffer);//use slot 1.
			con->DrawIndexed(mIndexCount, 0, 0);
			pCBuffer->Release();
		}
	}
}

//d3d_constant_set
