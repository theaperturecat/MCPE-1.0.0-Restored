
namespace mce
{
	class FrameBufferObject
	{
		void createFrameBuffer(, FrameBufferDescription&);
		void createFrameBufferAttachment(, FrameBufferAttachmentDescription&,Texture&);
	};
	class FrameBufferDescription
	{

	};
	class FrameBufferAttachmentDescription
	{

	};

	class RenderContext
	{
	public:
		void beginFrame();
		void endFrame();
	};
};