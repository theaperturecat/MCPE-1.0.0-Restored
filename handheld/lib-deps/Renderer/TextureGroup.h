#include "hal/interface/Texture.h"

namespace mce
{
	class TexturePair
	{
	public:
		void clear();
	};
	class TextureGroup
	{
	public:
		bool isLoaded(ResourceLocation&);
		TexturePtr getTexture(ResourceLocation*);
		void getTexturePair(TexturePair **);
		TexturePtr createEmptyTexture(TextureDescription &);
	};
}