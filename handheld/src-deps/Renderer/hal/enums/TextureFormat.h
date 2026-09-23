#pragma once

namespace mce
{
	enum class TextureFormat
	{
		R8G8B8_UNORM,
		R8G8B8A8_UNORM,
		B8G8R8A8_UNORM
	};


	class TextureHelper
	{
	public:
		static unsigned int textureFormatToByteStride(TextureFormat texFrmt)
		{
			return (texFrmt == TextureFormat::R8G8B8_UNORM ? 3 : 4);
		}
	};
};