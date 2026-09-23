#pragma once
//TAC todo
#include "renderer/hal/interface/Texture.h"
#include <vector>
#include "Resource/ResourceHelper.h"

namespace mce
{
	class TextureGroup;
	class RenderContext;

	class TexturePtr
	{
	public:
		TexturePtr& operator=(TexturePtr&& n)
		{
			_move(std::move(n));
			return *this;
		}
		TexturePtr()
		{
			group = nullptr;
			//z = "";
		}
		TexturePtr(TextureGroup& g, const ResourceLocation&);
		TexturePtr(TexturePtr& mm)
		{
			_move(std::move(mm));
		}
		~TexturePtr()
		{
			_deref();
		}

		void onGroupDestroyed()
		{
			t = nullptr;
			group = nullptr;
		}

		TextureGroup* getGroup()
		{
			return group;
		}

		Texture* get()
		{
			return t;
		}

		void onGroupReloaded();
		static const mce::TexturePtr NONE;

		TexturePtr& _move(TexturePtr&& n);

		Texture* operator*()
		{
			return t;
		}

		const Texture* operator*() const
		{
			return t;
		}

		Texture* operator->(void)
		{
			return t;
		}

		Texture const* operator->(void)const
		{
			return t;
		}

		ResourceLocation rl;

	protected:
		void _deref();

		operator bool() const
		{
			return t != nullptr;
		}



	private:
		TextureGroup* group;
		Texture* t;

	};
}