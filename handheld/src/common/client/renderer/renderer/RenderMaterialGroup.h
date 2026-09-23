#pragma once
#include "materialptr.h"
namespace mce
{
	class RenderMaterialGroup
	{
	public:
		/*class thingy
		{
		public:
			mce::MaterialPtr getMaterial(std::string)
			{
				return mce::MaterialPtr();
			}
		};*/
		static RenderMaterialGroup switchable;

		void _addRef(mce::MaterialPtr&)
		{

		}

		void _removeRef(mce::MaterialPtr&)
		{

		}

		RenderMaterial* getMaterial(const ResourceLocation &)
		{
			return nullptr;
		}

		MaterialPtr getMaterial(const std::string&)
		{
			return MaterialPtr();
		}
	};
}