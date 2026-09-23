#pragma once

#include "common_header.h"

#include "Renderer/TexturePtr.h"
#include "Core/Math/Color.h"
#include "client/renderer/texture/TextureData.h"
#include "Core/Resource/ResourceHelper.h"

namespace mce{
	class TextureGroup;
}
struct TextureUVCoordinateSet;

class TextureAtlasTile {
public:
	ResourceLocation location;
	Color overlay;
	std::vector<TextureUVCoordinateSet*> uvs;
	const TextureData* mTextureData;
	//UTexture2D* texture = nullptr;
	//mce::TexturePtr texture;
	ResourceLocation rl;
	//UTexture2D* textureEmissive = nullptr;

	bool quad;
	float mipFadeRate = 0.0f;
	Color mipFadeColor = Color::NIL;

	TextureAtlasTile(Color overlay, bool quad, ResourceLocation location);

	TextureAtlasTile(TextureAtlasTile&& rhs);

	TextureAtlasTile& operator=(TextureAtlasTile&& rhs);

	TextureAtlasTile(const TextureAtlasTile& rhs) = delete;

	TextureAtlasTile& operator=(const TextureAtlasTile&) = delete;
#ifdef TAC_COMPILE_UNREAL
	bool loadTexture(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources);
	bool loadTextureEmissive(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources);
#endif
	void loadTexture(mce::TextureGroup* texGroup);
	uint32_t getActualWidth() const;

	uint32_t getActualHeight() const;

	uint32_t getWidth() const;

	uint32_t getHeight() const;

	uint32_t getArea() const;
};