#pragma once
#include "materialptr.h"
#include <unordered_set>
#include <unordered_map>
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
		static RenderMaterialGroup common;
		static RenderMaterialGroup switchable;

		std::unordered_set<mce::MaterialPtr*> ml;
		//std::map<const ResourceLocation, TexturePair> mlll;
		void _addRef(mce::MaterialPtr& m)
		{
			ml.insert(&m);
		}

		void _removeRef(mce::MaterialPtr& l)
		{
			ml.erase(&l);
		}

		void _getMaterial(std::string&);

		void loadList(ResourceLocation &, int &);//ResourcePackManager

		RenderMaterial* _getMaterialPtr(const std::string & p)
		{
			if (materials.find(p) == materials.end())
				return nullptr;
			return materials[p];
		}

		RenderMaterial* getMaterialOrDefault(const std::string& p)
		{
			return materials.find(p) == materials.end() ? defaultMaterial : materials[p];
		}

		MaterialPtr getMaterial(const std::string& path)
		{
			return MaterialPtr(*this,path);
		}

		//tac transition functions
		void InitRenderMaterials();
		void LoadRenderMaterialFile(std::string rr);

		std::unordered_map<std::string, RenderMaterial*> materials;

		RenderMaterial* defaultMaterial = nullptr;




	};
}