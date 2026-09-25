//tac todo
#pragma once
#include "hal/enums/TextureFormat.h"
#include "hal/interface/RenderContext.h"
namespace mce
{
	struct TextureDescription
	{
		int mWidth;
		int mHeight;
		int a;
		//int unk1;//thing 1
		mce::TextureFormat mTextureFormat;
		int b;
		int c;
		unsigned int mMipMapCount;
		int unk2;

		int getSizeInBytes() const
		{
			return mWidth * mHeight * TextureHelper::textureFormatToByteStride(mTextureFormat);
		}
	};
	typedef TextureDescription ImageDescription;

	class Texture
	{
	public:

		Texture()
		{
			texid = 0;
		}

		TextureDescription texDesc;
		TextureDescription& getDescription()
		{
			return texDesc;
		}


		Texture & operator=(Texture&& other)
		{
			texid = other.texid;//theaperturecat
			texDesc = other.texDesc;
		}

		void subBuffer(RenderContext&, unsigned char*, int startx, int starty, int width, int height, unsigned int mipid);

		void bindTexture(RenderContext&, unsigned int, unsigned int) const;

		void createTexture(RenderContext&, TextureDescription&);

		void deleteTexture();
		unsigned int texid = -1;
		//TextureFormat frmt;
	};

	/*struct ImageDescription
	{
		int mWidth;
		int mHeight;
		mce::TextureFormat mTextureFormat;
		unsigned int mMipMapCount;

		int getSizeInBytes()
		{
			return mWidth * mHeight * TextureHelper::textureFormatToByteStride(mTextureFormat);
		}
	};*/

	/*class Texture
	{
	public:
		 TextureDescription getDescription();
		void subBuffer(RenderContextImmediate&, uint8_t *, int, int, uint32_t, uint32_t, uint32_t)
	};*/
}