#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include <stb_image.h>
#include "image.h"

namespace Assets
{
	Image::Image(const std::string & file)
		: 
		mImageData(nullptr)
	{
		mImageData =
			stbi_load(file.c_str(), &mWidth, &mHeight, &mBitsPerChannel, 0);
	}
	Image::~Image()
	{
		stbi_image_free(mImageData);
	}
}