#include "textureptr.h"
#include "TextureGroup.h"
namespace mce
{
TexturePtr & TexturePtr::_move(TexturePtr && n)
{
	group = n.group;
	t = n.t;

	std::swap(n.rl, rl);

	n._deref();

	group->_addRef(*this);
	return *this;//?
}


TexturePtr::TexturePtr(TextureGroup& g, const ResourceLocation& loc)
{
	group = &g;
	t = nullptr;
	rl = loc;

	g._addRef(*this);

	t = group->_getTexture(rl);
}

void TexturePtr::onGroupReloaded()
{
	t = group->_getTexture(rl);
}

void TexturePtr::_deref()
{
	if (group)
	{
		group->_removeRef(*this);
		group = nullptr;
	}
	t = nullptr;
}


};
