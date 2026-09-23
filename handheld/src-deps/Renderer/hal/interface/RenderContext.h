//tac todo
#pragma once
namespace mce
{
	class Texture;
	class FrameBufferDescription
	{

	};
		class FrameBufferAttachmentDescription
	{

	};

		class RenderContext
		{
		public:
			void beginFrame()
			{

			}
			void endFrame()
			{

			}
		};

	class FrameBufferObject
	{
		void createFrameBuffer(RenderContext&, FrameBufferDescription&);
		void createFrameBufferAttachment(RenderContext&, FrameBufferAttachmentDescription&,Texture&);
	};
};