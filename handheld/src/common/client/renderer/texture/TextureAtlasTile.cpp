#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlasTile.h"
//#include "Renderer/HAL/Interface/Texture.h"
#include "Renderer/TextureGroup.h"
#include "client/renderer/texture/TextureAtlas.h"
//#include "lovika/io/TextureFile.h"
//#include "ImageWrapper.h"

TextureAtlasTile::TextureAtlasTile(Color overlay, bool quad, ResourceLocation location)
	: overlay(overlay)
	, quad(quad)
	, location(location)
{
}

TextureAtlasTile::TextureAtlasTile(TextureAtlasTile &&rhs)
	: overlay(rhs.overlay)
	, quad(rhs.quad)
	, location(rhs.location)
	, mipFadeRate(rhs.mipFadeRate)
	, mipFadeColor(rhs.mipFadeColor)
	, uvs(std::move(rhs.uvs))
	//, texture(rhs.texture)
	, mTextureData(rhs.mTextureData)
{
}

TextureAtlasTile& TextureAtlasTile::operator=(TextureAtlasTile &&rhs) {
	if (this != &rhs) {
		overlay = rhs.overlay;
		quad = rhs.quad;
		location = rhs.location;
		mipFadeRate = rhs.mipFadeRate;
		mipFadeColor = rhs.mipFadeColor;
		uvs = rhs.uvs;
		//texture = std::move(rhs.texture);
		mTextureData = rhs.mTextureData;
	}
	return *this;
}

void TextureAtlasTile::loadTexture(mce::TextureGroup* texGroup)
{
	ResourceLocation rl;
	optional_ref<mce::TexturePair> texturlle = texGroup->getTexturePair(location);

	TextureData * d = texturlle.unwrap().getTextureData();

	mTextureData = d;

	d->getImageDescription().getSizeInBytes();

	d->getDataPacked();
}

uint32_t TextureAtlasTile::getActualWidth() const {
	return std::max((uint32_t)mTextureData->getImageDescription().mWidth, TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getActualHeight() const {
	return std::max((uint32_t)mTextureData->getImageDescription().mHeight, TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getWidth() const {
	return std::max((uint32_t)mTextureData->getImageDescription().mWidth * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getHeight() const {
	return std::max((uint32_t)mTextureData->getImageDescription().mHeight * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getArea() const {
	return getWidth() * getHeight();
}