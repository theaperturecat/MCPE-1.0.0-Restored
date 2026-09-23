//tac todo
#pragma once
namespace mce
{

	enum class RenderFeature
	{
		HalfFloats,
		Mipmaps,
	};

	class RenderDevice
	{
	public:
		static RenderDevice& getInstance()
		{
			static RenderDevice rl;
			return rl;
		}
		int getMaxVertexCount()
		{
			return 100000;
		}
		bool checkFeatureSupport(RenderFeature)
		{
			return false;
		}
	};
}