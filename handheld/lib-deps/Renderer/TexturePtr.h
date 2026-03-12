namespace mce
{
	class Texture;
	class TexturePtr
	{
	public:
		TextureAtlasTile& operator=(Texture * rhs);
		operator bool() const;
		Texture* operator->() const;
	};
}