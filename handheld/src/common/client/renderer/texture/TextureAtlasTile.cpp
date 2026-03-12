//tac todo
#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlasTile.h"
#include "Renderer/HAL/Interface/Texture.h"
#include "Renderer/TextureGroup.h"
#include "client/renderer/texture/TextureAtlas.h"
#include <cmath>
//#include "lovika/io/TextureFile.h"

TextureAtlasTile::TextureAtlasTile(Color overlay, bool quad, ResourceLocation location)
	: overlay(overlay)
	, quad(quad)
	, location(location)
	, texture(nullptr) {
}

TextureAtlasTile::TextureAtlasTile(TextureAtlasTile &&rhs)
	: overlay(rhs.overlay)
	, quad(rhs.quad)
	, location(rhs.location)
	, mipFadeRate(rhs.mipFadeRate)
	, mipFadeColor(rhs.mipFadeColor)
	, uvs(std::move(rhs.uvs))
	, texture(rhs.texture) {
}

TextureAtlasTile& TextureAtlasTile::operator=(TextureAtlasTile &&rhs) {
	if (this != &rhs) {
		overlay = rhs.overlay;
		quad = rhs.quad;
		location = rhs.location;
		mipFadeRate = rhs.mipFadeRate;
		mipFadeColor = rhs.mipFadeColor;
		uvs = rhs.uvs;
		texture = rhs.texture;
	}
	return *this;
}

void TextureAtlasTile::loadTexture(mce::TextureGroup textures/*std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources*/) {
	//ResourceLocation locations[48];
	mce::TexturePtr ptr;
	texture = textures.getTexture(ptr);
}

uint32_t TextureAtlasTile::getActualWidth() const {
	DEBUG_ASSERT(texture, "Texture not loaded");
	mce::TextureDescription pTVar2 = texture->getDescription();
	return std::max((uint32_t)pTVar2.mHeight, TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getActualHeight() const {
	DEBUG_ASSERT(texture,"Texture not loaded");
	mce::TextureDescription pTVar2 = texture->getDescription();
	return std::max((uint32_t)pTVar2.mHeight, TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getWidth() const {
	DEBUG_ASSERT(texture, "Texture not loaded");
	mce::TextureDescription pTVar2 = texture->getDescription();
	return std::max((uint32_t)pTVar2.mWidth * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getHeight() const {
	DEBUG_ASSERT(texture, "Texture not loaded");
	mce::TextureDescription pTVar2 = texture->getDescription();
	return std::max((uint32_t)pTVar2.mWidth * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getArea() const {
	return getWidth() * getHeight();
}