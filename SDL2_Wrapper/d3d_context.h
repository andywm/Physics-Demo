/*------------------------------------------------------------------------------
author: andywm, 2016
description:
defines the class that manages the d3d11 subsystems, encapsulates as a context
...
------------------------------------------------------------------------------*/

#ifndef D3D_CONTEXT_H
#define D3D_CONTEXT_H
#include <map>
#include <array>
#include <wrl.h>
#include<d3d11.h>
#include<glm/glm.hpp>
#include "window_archtype.h"
#include "mesh.h"
#include "d3d_shaders.h"
#include "d3d_pipeline_state.h"
#include "d3d_gpu_geometry.h"
#include "sys_shader.h"
#include "d3d_texture.h"

namespace Render
{
	enum class ACTION { CLEAR, GEOMETRY, COMMIT_VP, COMMIT_SHADER };
	namespace d3d11
	{
		class D3DContext
		{
		/*inteface*/
		public:
			//actions
			void execute(const ACTION act);
			void setShaders();
			void swap();
			inline PipelineState &
			pipeline()
			{
				return mPipeline;
			}
		private:
			std::shared_ptr<Shader> mVSforInputAssembly;
			void renderClear();
			void renderGeometry();

		//members//
		private:
			class ResourceManager
			{
			private:
				D3DContext & mContext;
				struct MeshRecord
				{
					std::shared_ptr<Assets::Mesh> sysFormat;
					std::shared_ptr<Geometry> gpuFormat;
				};
				std::vector<MeshRecord> mMeshes;

				std::vector<std::shared_ptr<Assets::SysShader>> mShaderPaths;
				std::map<std::shared_ptr<Assets::SysShader>, 
										std::shared_ptr<Shader>> mShaders;
				std::map <std::shared_ptr<Assets::Image>,
										std::shared_ptr<Texture>> mTextures;
			public:
				explicit ResourceManager(D3DContext & parent);
				ResourceManager& operator=(const ResourceManager&) = delete;
				~ResourceManager() = default;
				std::shared_ptr<Geometry> geometry(
						std::shared_ptr<Assets::Mesh> m);

				std::shared_ptr<Render::d3d11::Texture>
				setTexture(std::shared_ptr<Assets::Image> img);

				void addShaderPath(const 
					std::shared_ptr<Assets::SysShader> shade);
				void enableShaders(const 
					std::shared_ptr<Assets::SysShader> sd_set);
				std::shared_ptr<Shader> &
					getShader(const std::shared_ptr<Assets::SysShader> sd_set);
			}mAssets;

			using rtv_array_type =
				std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>,
				D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT>;

			const unsigned cmMaxRenderTargets =
				D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
			Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
			Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
			Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
			std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 2> 
				mRTV;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mZBuffer;
			D3D11_VIEWPORT mViewport;
			std::unique_ptr<Texture> mTexRTV1, mTexRTV2, mDepthBuffer;
			Render::d3d11::PipelineState mPipeline; 
		//methods//
		private:
			void createDepthStencilView();
			void createRenderTargetView();
			void createBlendState();
			bool initialiseD3D(const HWND hwnd, const glm::ivec2 & winSize);
			void setViewport(const glm::ivec2 & winSize);
		//methods//
		public:
			D3DContext(const HWND hwnd, const glm::ivec2 & winSize);
			D3DContext(D3DContext & ctx) = delete;
			D3DContext& operator=(const D3DContext&) = delete;
			~D3DContext();
			
			inline Microsoft::WRL::ComPtr<IDXGISwapChain> &
			swapChain() 
			{
				return mSwapChain;
			}
			inline const Microsoft::WRL::ComPtr<ID3D11Device> &
			device() const
			{
				return mDevice;
			}

			inline const Microsoft::WRL::ComPtr<ID3D11DeviceContext> &
			deviceContext() const
			{
				return mContext;
			}
			
			/*management*/
			void updateViewport();
			void resize(const glm::ivec2 & size);
			/*tools*/
			//const Microsoft::WRL::ComPtr<ID3D11Device> & device();
			//const Microsoft::WRL::ComPtr<ID3D11DeviceContext> & deviceContext();	
		};
	}
}
#endif // D3D_CONTEXT_H

//	DXGI_FORMAT format();
// DXGI_FORMAT format(DXGI_FORMAT frm);