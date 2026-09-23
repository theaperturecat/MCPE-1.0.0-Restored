//tac mostly
#pragma once
#include <string>
#include <vector>
#include "rendermaterial.h"
//#include "../../src/common/client/renderer/renderer/RenderMaterialGroup.h"
#include "Resource/ResourceHelper.h"
namespace mce
{
	class RenderMaterialGroup;
	class MaterialPtr
	{
	public:

		MaterialPtr& operator=(MaterialPtr&& n)
		{
			_move(std::move(n));
			return *this;
		}
		MaterialPtr()
		{
			group = nullptr;
			//z = "";
		}
		MaterialPtr(RenderMaterialGroup& g, ResourceLocation& llll);
		MaterialPtr(MaterialPtr& mm)
		{
			//z = "";
			_move(std::move(mm));
		}
		~MaterialPtr()
		{
			_deref();
		}
		void onGroupReloaded();
		static mce::MaterialPtr const NONE;
		RenderMaterial const* operator->(void)const
		{
			return matptr;
		}
	protected:
		void _deref();
		MaterialPtr& _move(MaterialPtr&& n);
		RenderMaterialGroup* group;
		RenderMaterial* matptr;
		ResourceLocation lll;
		//std::string z;
	};
};