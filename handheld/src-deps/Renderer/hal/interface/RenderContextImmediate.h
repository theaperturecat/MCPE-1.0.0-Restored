//tac todo

#pragma once
#include "rendercontext.h"
namespace mce
{
	class RenderContextImmediate
	{
	public:
		static RenderContext& get()
		{
			static RenderContext l;
			return l;
		}
	};
}