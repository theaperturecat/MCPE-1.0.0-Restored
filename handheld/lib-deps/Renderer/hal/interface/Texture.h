#include "Renderer/TexturePtr.h"
#include "Renderer/hal/enums/TextureFormat.h"

namespace mce
{
	class TextureDescription
	{
	public:
		int mWidth;
		int mHeight;
		TextureFormat mFormat;
		int getSizeInBytes();
	};

	class Texture
	{
	public:
		 TextureDescription getDescription();
		void subBuffer(RenderContextImmediate&, uint8_t *, int, int, uint32_t, uint32_t, uint32_t)
	};
}