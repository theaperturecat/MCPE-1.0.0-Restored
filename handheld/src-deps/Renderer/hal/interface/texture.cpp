#include "Textureptr.h"
#include "RenderContext.h"
#include <glad/glad.h>
namespace mce
{
	unsigned int cpuformats[] = {
		GL_RGB,
		GL_RGBA,
		GL_BGRA,
	};

	unsigned int gpuformats[] = {
	GL_RGB8,
	GL_RGBA8,
	GL_RGBA8,
	};

	void Texture::subBuffer(RenderContext& ctxt, uint8_t* bufferData, int startx, int starty, int width, int height, unsigned int mipid)
	{
		if (texid == -1)
			return;
		//bindTexture();
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexSubImage2D(GL_TEXTURE_2D, mipid, startx, starty, width, height, cpuformats[(unsigned int)texDesc.mTextureFormat], GL_UNSIGNED_BYTE, bufferData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture::bindTexture(RenderContext&, unsigned int n, unsigned int) const//whats the other param???
	{
		if (texid == -1)
			return;
		glActiveTexture(GL_TEXTURE0+n);//just a guess fixme
		glBindTexture(GL_TEXTURE_2D, texid);
	}

	void Texture::createTexture(RenderContext& ctxt, TextureDescription& desc)
	{
		texDesc = desc;

		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, gpuformats[(unsigned int)desc.mTextureFormat], desc.mWidth, desc.mHeight, 0,cpuformats[(unsigned int)desc.mTextureFormat],GL_UNSIGNED_BYTE , nullptr);
	
		//glGenerateMipmap(GL_TEXTURE_2D);
	
	}

	void Texture::deleteTexture()
	{
		if (texid == -1)
			return;
		glDeleteTextures(1, &texid);
		texid = -1;
	}
};