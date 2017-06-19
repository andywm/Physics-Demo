#ifndef IMAGE_H
#define IMAGE_H
#include<string>

namespace Assets
{
	class Image
	{
	private:
		int mWidth, mHeight, mBitsPerChannel;
		unsigned char * mImageData;
	public:
		explicit Image(const std::string & file);
		Image& operator=(const Image&) = delete;
		Image(const Image&) = delete;
		~Image();

		inline const unsigned char * 
		raw() const
		{
			return mImageData;
		}

		inline const unsigned int 
		bitDepth() const
		{
			return mBitsPerChannel;
		}

		inline const unsigned int 
		width() const
		{
			return mWidth;
		}

		inline const unsigned int
		height() const
		{
			return mHeight;
		}
	};
}
#endif 