//#include "materialptr.h"
#include "../../src/common/client/renderer/renderer/RenderMaterialGroup.h"

namespace mce
{
	MaterialPtr& MaterialPtr::_move(MaterialPtr&& n)
	{
		group = n.group;
		matptr = n.matptr;

		std::swap(n.lll, lll);

		n._deref();

		group->_addRef(*this);

		return *this;
	}

	MaterialPtr::MaterialPtr(RenderMaterialGroup& g, const std::string& llll)
	{
		group = &g;
		matptr = nullptr;

		lll = llll;

		g._addRef(*this);

		matptr = group->_getMaterialPtr(lll);
	}

	void MaterialPtr::onGroupReloaded()
	{
		matptr = group->_getMaterialPtr(lll);
	}

	void MaterialPtr::_deref()
	{
		if (group)
		{
			group->_removeRef(*this);
			group = nullptr;
		}
		matptr = nullptr;
	}

};