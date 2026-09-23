//TAC todo
#pragma once
#include "hal/enums/TextureFormat.h"
#include "TexturePtr.h"

#include "Core/Utility/optional_ref.h"
#include "../../src/common/client/renderer/texture/TextureData.h"
#include "hal/interface/RenderContextImmediate.h"

#include <map>
#include <unordered_set>

class ResourceLocation;

TextureData mcperes_loadTexture(const ResourceLocation& resourceLocation);

namespace mce
{
	class TexturePair
	{
	public:
		Texture tex;
		TextureData dt;

		TexturePair()//? does this exist?
		{

		}

		TexturePair(TextureData&& ll) : tex(), dt(std::move(ll))
		{
			
		}

		TexturePair(unsigned int al, unsigned int bl) : tex(), dt(al,bl)
		{
		
		}

		TexturePair(TexturePair&&)
		{

		}

		void TexturePair::operator=(TexturePair&& other)
		{
			dt = std::move(other.dt);
			tex = std::move(other.tex);
		}

		void clear()
		{
			tex.deleteTexture();
			dt.clear();
		}

		TextureData * getTextureData()
		{
			return &dt;
		}

		void unload()
		{
			tex.deleteTexture();
		}
	};

	class TextureGroup
	{
	public:
		std::unordered_set<mce::TexturePtr*> ml;
		std::map<const ResourceLocation, TexturePair> mlll;
		void _addRef(mce::TexturePtr& m)
		{
			ml.insert(&m);
		}

		void _removeRef(mce::TexturePtr& l)
		{
			ml.erase(&l);
		}

		bool isLoaded(ResourceLocation& loc)
		{
			return mlll.find(loc) != mlll.end();
		}

		Texture* _getTexture(const ResourceLocation& lo)
		{
			return &mlll[lo].tex;// is this right?
		}

		TexturePtr getTexture(const ResourceLocation& l)
		{
			return TexturePtr(*this,l);//that easy, i think???
		}

		optional_ref<TexturePair> getTexturePair(const TexturePtr &pt)
		{
			return getTexturePair(pt.rl);
			//auto& n = mlll.find(pt.rl);
			//if (n != mlll.end())
			//	return optional_ref<TexturePair>(n->second);
			//else
			//	return optional_ref<TexturePair>();
		}

		optional_ref<TexturePair> getTexturePair(const ResourceLocation&loc)
		{
			auto &n = mlll.find(loc);
			if (n != mlll.end())
			{
				return optional_ref<TexturePair>(n->second);
			} 
			else
			{
				mlll[loc].dt = std::move(mcperes_loadTexture(loc));
				TextureDescription md = mlll[loc].dt.getImageDescription();
				mlll[loc].tex.createTexture(RenderContextImmediate::get(), md);
				return optional_ref<TexturePair>(mlll[loc]);
			}
		}
		TexturePtr createEmptyTexture(const ResourceLocation& loc,  TextureDescription& desc)
		{
			mlll[loc].tex.createTexture(RenderContextImmediate::get(),desc);
			TexturePtr l(*this, loc);
			return l;
		}

		static bool mCanLoadTextures;
	};
}