#pragma once
#include <d3d11.h>
#include<wrl.h>
#include "d3d_context.h"
#include "image.h"
#include "d3d_texture.h"

namespace Render
{
	namespace d3d11
	{
		Texture::Texture(D3DContext * const context,
			const bool allocateFromBackBuffer)
			:
			mBindFlags(0)
		{
			createAsTarget(context, allocateFromBackBuffer);
		}

		Texture::Texture(D3DContext * const context, 
			const Assets::Image & img)
			:
			mBindFlags(0)
		{
			createAsAsset(context, img);
		}

		D3D11_TEXTURE2D_DESC
		Texture::textureDescription(D3DContext * const context) const
		{
			DXGI_SWAP_CHAIN_DESC swapDesc;
			context->swapChain()->GetDesc(&swapDesc);

			D3D11_TEXTURE2D_DESC texDesc;

			texDesc.Width = swapDesc.BufferDesc.Width;
			texDesc.Height = swapDesc.BufferDesc.Height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = mFormat;
			texDesc.SampleDesc = swapDesc.SampleDesc;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = mBindFlags;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			return texDesc;
		}

		void Texture::createAsAsset(D3DContext * const contex,
			const Assets::Image & img)
		{
			auto texDesc = textureDescription(contex);
			texDesc.Height = img.height();
			texDesc.Width = img.width();
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = img.raw();
			data.SysMemPitch = img.bitDepth() * img.width();
			data.SysMemSlicePitch = 0;

			//HRESULT hr =
			contex->device()->CreateTexture2D(&texDesc,
				&data, &mTexture);

			mFormat = texDesc.Format;
		}

		void
		Texture::createAsTarget(D3DContext * const context, 
			const bool allocFromBackBuffer)
		{
			HRESULT hr(S_FALSE);

			if (allocFromBackBuffer)
			{
				hr = context->swapChain()->GetBuffer(0,
					__uuidof(ID3D11Texture2D),
					&mTexture);
			}
			else
			{
				auto texDesc = textureDescription(context);
				texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				texDesc.Format = DXGI_FORMAT_D32_FLOAT;
				mFormat = texDesc.Format;
				mBindFlags = texDesc.BindFlags;

				if (mBindFlags & D3D11_BIND_DEPTH_STENCIL)
				{
					if (mBindFlags & D3D11_BIND_SHADER_RESOURCE)
					{
						switch (texDesc.Format)
						{
						case DXGI_FORMAT_D16_UNORM:
							texDesc.Format = DXGI_FORMAT_R16_TYPELESS;
							break;

						case DXGI_FORMAT_D24_UNORM_S8_UINT:
							texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
							break;

						default:
							texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
							break;
						}
					}
					hr = context->device()->CreateTexture2D(&texDesc,
						nullptr, &mTexture);
				}
			}
		}
	}
}