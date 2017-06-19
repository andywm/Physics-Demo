#pragma once
#include <d3d11.h>
#include<wrl.h>
#include "d3d_context.h"
#include "image.h"

namespace Render
{
	namespace d3d11
	{
		class D3DContext;
		class Texture
		{
		public:
			Texture(D3DContext * const context, 
				const bool allocateFromBackBuffer);
			Texture(D3DContext * const context, 
				const Assets::Image & img);
			inline const Microsoft::WRL::ComPtr<ID3D11Texture2D> &
			texture() const
			{
				return mTexture;
			}
			inline const DXGI_FORMAT &
			format() const 
			{
				return mFormat;
			}
			inline const glm::vec2 &
			size() const
			{
				return mSize;
			}
			~Texture() = default;
			
		private:
			//allocate from backbuffer, heap (with intent) write or read.

			Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
			DXGI_FORMAT mFormat;
			glm::vec2 mSize;
			unsigned mBindFlags;

		private:
			void createAsTarget(D3DContext * const context, 
				const bool allocFromBackBuffer);
			void createAsAsset(D3DContext * const context,
				const Assets::Image & img);
			D3D11_TEXTURE2D_DESC textureDescription(
				D3DContext * const context) const;
		};
	}
}