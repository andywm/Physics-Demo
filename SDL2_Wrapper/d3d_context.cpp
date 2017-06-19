#include <algorithm>
#include <d3d11.h>
#include <array>
#include <string>
#include <glm\vec2.hpp>
#include "d3d_shaders.h"
#include "d3d_context.h"
#include "mesh.h"
#include "d3d_texture.h"
#include <exception>

namespace Render
{
	namespace d3d11
	{
		D3DContext::D3DContext(const HWND hwnd, const glm::ivec2 & winSize)
			:mSwapChain(nullptr),
			mDevice(nullptr),
			mContext(nullptr),
			mAssets(*this),
			cmMaxRenderTargets(8)
		{
			initialiseD3D(hwnd, winSize);
			createRenderTargetView();
			setViewport(winSize);
		}

		bool D3DContext::initialiseD3D(const HWND hwnd, const glm::ivec2 & winSize)
		{
			//set flags.
			const UINT createDeviceFlags = 0;
#ifdef _DEBUG
			//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG
			const std::array<D3D_DRIVER_TYPE,2> driverTypes =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_SOFTWARE
			};
			const std::array<D3D_FEATURE_LEVEL, 3> featureLevels
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};
			const UINT featureLevelCount = featureLevels.size();

			//swap chain 
			DXGI_SWAP_CHAIN_DESC swapDesc;
			memset(&swapDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
			swapDesc.BufferCount = 2; //double buffered

			swapDesc.BufferDesc.Width = winSize.x;
			swapDesc.BufferDesc.Height = winSize.y;
			swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapDesc.BufferDesc.RefreshRate.Numerator = 240;//60fps
			swapDesc.BufferDesc.RefreshRate.Denominator = 4;
			swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapDesc.OutputWindow = hwnd;
			swapDesc.Windowed = true;
			swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapDesc.BufferDesc.ScanlineOrdering =
				DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapDesc.SampleDesc.Count = 1;
			swapDesc.SampleDesc.Quality = 0;
			swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			

			bool success = false;
			for (const auto driver : driverTypes)
			{
				const HRESULT hr = D3D11CreateDeviceAndSwapChain(
					nullptr,					//adpater
					driver,						//driver type
					nullptr,					//software
					createDeviceFlags,			//flags
					&featureLevels[0],			//feature level
					featureLevelCount,			//size
					D3D11_SDK_VERSION,			//sdk
					&swapDesc,					//swap chain desc
					&mSwapChain,				//swap
					&mDevice,					//device
					nullptr,					//feature level
					&mContext					//context
				);

				if (hr == S_OK)
				{
					success = true;
					break;
				}
			}
			resize(winSize);
			return success;
		}

		void
		D3DContext::resize(const glm::ivec2 & size)
		{
			//mContext->ClearState();
			mRTV[0] = nullptr;
			mRTV[1] = nullptr;
			mTexRTV1 = nullptr;
			mTexRTV2 = nullptr;

			DXGI_SWAP_CHAIN_DESC swapDesc;
			mSwapChain->GetDesc(&swapDesc);

			swapDesc.BufferDesc.Width = std::min(D3D11_VIEWPORT_BOUNDS_MAX,
				std::max(0, size.x));
			
			swapDesc.BufferDesc.Height = std::min(D3D11_VIEWPORT_BOUNDS_MAX,
				std::max(0, size.y));

			//const HRESULT hr = 
				mSwapChain->ResizeBuffers(swapDesc.BufferCount,
					swapDesc.BufferDesc.Width,
					swapDesc.BufferDesc.Height,
					swapDesc.BufferDesc.Format,
					swapDesc.Flags);

			createRenderTargetView();
			updateViewport();
			//createRenderTargetView();
		}

		void
		D3DContext::updateViewport()
		{
			DXGI_SWAP_CHAIN_DESC info;
			mSwapChain->GetDesc(&info);

			D3D11_VIEWPORT viewport;

			viewport.TopLeftX = 0.0;
			viewport.TopLeftY = 0.0;
			viewport.Width = static_cast<float>(info.BufferDesc.Width);
			viewport.Height = static_cast<float>(info.BufferDesc.Height);
			viewport.MinDepth = 0.0;
			viewport.MaxDepth = 1.0;

			mContext->RSSetViewports(1, &viewport);
		}
		void
		D3DContext::setViewport(const glm::ivec2 & winSize)
		{
			//make viewport
			mViewport.Width = static_cast<float>(winSize.x);
			mViewport.Height = static_cast<float>(winSize.y);
			mViewport.TopLeftX = 0;
			mViewport.TopLeftY = 0;
			mViewport.MinDepth = 0.0f;
			mViewport.MaxDepth = 1.0f;

			//bind it
			mContext->RSSetViewports(1, &mViewport);
		}

		void 
		D3DContext::swap()
		{
			try
			{
				mSwapChain->Present(0, 0);
				mContext->OMSetRenderTargets(
					1, mRTV[0].GetAddressOf(), mZBuffer.Get());

				mContext->ClearDepthStencilView(
					mZBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
			catch (std::exception &) {}
		}
		//resize

		D3DContext::~D3DContext()
		{
			try
			{
				mSwapChain->SetFullscreenState(false, nullptr);
				mContext->ClearState();
			}
			catch(const std::exception&)
			{
				
			};
		}

		/*----------------------------------------------------------------------
		interface
		----------------------------------------------------------------------*/

		void 
		D3DContext::setShaders()
		{
			auto shaderProfiles = mPipeline.shaders();
			for (const auto shaderProfile : shaderProfiles)
			{
				mAssets.enableShaders(shaderProfile);
				if (shaderProfile->identity()
					== Assets::SysShader::IDENTITY::VS)
				{
					mVSforInputAssembly = mAssets.getShader(shaderProfile); 
				}
			}
		}

		void
		D3DContext::renderClear()
		{
			const glm::vec4 & colour = mPipeline.clearColour();
			const std::array<float,4> col 
				= { colour.x, colour.y, colour.z, colour.w };



			mContext->ClearRenderTargetView(mRTV[0].Get(), &col[0]);
		}

	

		void
		D3DContext::execute(const ACTION act)
		{
			switch (act)
			{
			case ACTION::GEOMETRY:
				{
					renderGeometry();
					break;
				}
			case ACTION::CLEAR:
				{
					renderClear();
					break;
				}
			case ACTION::COMMIT_VP:
				{
					ID3D11Buffer *pCBuffer;
					D3D11_BUFFER_DESC bd;
					ZeroMemory(&bd, sizeof(bd));
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(glm::mat4) * 4;
					bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					mDevice->CreateBuffer(&bd, nullptr, &pCBuffer);

					struct subresource
					{
						glm::mat4 view;
						glm::mat4 projection;
						glm::mat4 view_it;
						glm::mat4 projection_it;
					}const data{
						pipeline().view(),
						pipeline().projection(),
						glm::transpose(glm::inverse(pipeline().view())),
						glm::transpose(glm::inverse(pipeline().projection()))
					};
					mContext->UpdateSubresource(pCBuffer, 0, nullptr, &data, 0, 0);
					mContext->VSSetConstantBuffers(0, 1, &pCBuffer);//use slot 0
					mContext->PSSetConstantBuffers(0, 1, &pCBuffer);//use slot 0
					pCBuffer->Release();
					break;
				}
			case ACTION::COMMIT_SHADER:
				{
					setShaders();
					break;
				}
			}
		}

		void
			D3DContext::renderGeometry()
		{
			if (mPipeline.texture() != nullptr)
			{
				auto const resource =
					mAssets.setTexture(mPipeline.texture());

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				ZeroMemory(&srvDesc, sizeof(srvDesc));
				srvDesc.Format = resource->format();

				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D = D3D11_TEX2D_SRV{ 0,1 };

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
				//const HRESULT hr = 
				mDevice->CreateShaderResourceView(
					resource->texture().Get(),
					&srvDesc,
					&srv);
			}

			ID3D11Buffer *pCBuffer;
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(glm::mat4) * 4;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			mDevice->CreateBuffer(&bd, nullptr, &pCBuffer);

			struct subresource
			{
				glm::vec4 diffuseColour;
				glm::vec4 ambientColour;
				glm::vec4 specularColour;
				float specularExponent;
			};
			if (mPipeline.material() != nullptr)
			{
				const subresource data{
					mPipeline.material()->diffuse(),
					mPipeline.material()->ambient(),
					mPipeline.material()->specular(),
					mPipeline.material()->specularExponent() 
				};
				mContext->UpdateSubresource(pCBuffer, 0, nullptr, &data, 0, 0);
			}
			else
			{
				const subresource data{
					glm::vec4(1,1,0,1),
					glm::vec4(1, 1, 0, 1),
					glm::vec4(1, 1, 0, 1),
					5.0f
				};
				mContext->UpdateSubresource(pCBuffer, 0, nullptr, &data, 0, 0);
			}
			
			//mContext->VSSetConstantBuffers(0, 1, &pCBuffer);//use slot 0
			mContext->PSSetConstantBuffers(2, 1, &pCBuffer);//use slot 2
			pCBuffer->Release();

			//setShaders();
			mAssets.geometry(mPipeline.geometry())->setInputLayout(
												*this, *mVSforInputAssembly);
			
			mAssets.geometry(mPipeline.geometry())->draw(*this);
		}

		void 
		D3DContext::createRenderTargetView()
		{
			createDepthStencilView();
			mTexRTV1 = std::unique_ptr<Texture>(new Texture(this, true));
			mTexRTV2 = std::unique_ptr<Texture>(new Texture(this, true));
	
			mDevice->CreateRenderTargetView(
				mTexRTV1->texture().Get(), nullptr, &mRTV[0]);
			mDevice->CreateRenderTargetView(
				mTexRTV2->texture().Get(), nullptr, &mRTV[1]);
			mContext->OMSetRenderTargets(
				1, mRTV[0].GetAddressOf(), mZBuffer.Get());

			createBlendState();
		}

		void 
		D3DContext::createBlendState()
		{
			D3D11_BLEND_DESC bsd;
			ZeroMemory(&bsd, sizeof(D3D11_BLEND_DESC));
			bsd.AlphaToCoverageEnable = FALSE;
			bsd.IndependentBlendEnable = FALSE;
			for (uint i=0; i<mRTV.size(); i++)
			{
				bsd.RenderTarget[i].BlendEnable = TRUE;
				bsd.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				bsd.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				bsd.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
				bsd.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
				bsd.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
				bsd.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				bsd.RenderTarget[i].RenderTargetWriteMask =
					D3D11_COLOR_WRITE_ENABLE_ALL;
			}

			//HRESULT hr =
			mDevice->CreateBlendState(&bsd, &mBlendState);
			mContext->OMSetBlendState(mBlendState.Get(), NULL, 0xFFFFFF);
		}

		void
		D3DContext::createDepthStencilView()
		{
			//mRTVallocFrom = ALLOC_MODE::TEXTURE2D;
			//mDepthBuffer.bindFlags = D3D11_BIND_DEPTH_STENCIL;
			//mDepthBuffer.format = DXGI_FORMAT_D32_FLOAT;
			//createTexture(mDepthBuffer);
			mDepthBuffer = nullptr;
			mDepthBuffer = std::unique_ptr<Texture>(new Texture(this, false));

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
			ZeroMemory(&dsvd, sizeof(dsvd));

			dsvd.Format = DXGI_FORMAT_D32_FLOAT;
			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			//const HRESULT hr = 
			mDevice->CreateDepthStencilView(
				mDepthBuffer->texture().Get(),
				&dsvd, &mZBuffer);
			//pDepthBuffer->Release();
			//mRTVallocFrom = ALLOC_MODE::BACK_BUFFER;
		}
	}
}


//do a general refactor on how textures are handled...
//that alloc mode flag has to go...













/*
DXGI_FORMAT D3DContext::format()
{
DXGI_SWAP_CHAIN_DESC info;
mSwapChain->GetDesc(&info);

return info.BufferDesc.Format;
}

DXGI_FORMAT D3DContext::format(DXGI_FORMAT frm)
{
DXGI_SWAP_CHAIN_DESC info;
mSwapChain->GetDesc(&info);

DXGI_FORMAT const result(info.BufferDesc.Format);
info.BufferDesc.Format = frm;

HRESULT const hr(mSwapChain->ResizeBuffers(info.BufferCount,
info.BufferDesc.Width,
info.BufferDesc.Height,
info.BufferDesc.Format,
info.Flags));

//check s_ok...

return result;
}*/