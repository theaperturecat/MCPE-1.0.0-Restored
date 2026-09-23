#include "Dungeons.h"

//theaperturecat - this file still needs a LOT of work
//good thing its not... required exactlty to display a meshed chunk
//at least, not in pink and black lol


#include "client/renderer/texture/TextureAtlas.h"
//#include "platform/AppPlatform.h"
#include "Core/Resource/ResourceHelper.h"
#include "util/range.h"

#include "client/renderer/block/BlockGraphics.h"
#include "util/JSONUtils.h"

#if !defined(MCPE_HEADLESS)
#include "client/renderer/renderer/Tessellator.h"
#include "Renderer/HAL/Enums/RenderFeature.h"
#include "Renderer/HAL/Enums/BindFlagsBit.h"
#include "Renderer/HAL/Interface/RenderContextImmediate.h"
#include "Renderer/HAL/Interface/RenderContext.h"
#include "Renderer/HAL/Interface/RenderDevice.h"
#include "Renderer/TextureGroup.h"
#endif

#include <glm/gtc/type_precision.hpp>
//#include <glm/core/func_common.hpp>
//#include <glm/gtx/fast_square_root.hpp>

#include "Core/Utility/optional_ref.h"

const uint32_t TextureAtlas::MIN_TILE_SIDE = 16;

TextureAtlas::~TextureAtlas() {

}

// #ifndef MCPE_HEADLESS

TextureAtlas::TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures)
//TextureAtlas::TextureAtlas()
	: mMissingTextureItem("", 255) // HACK: 255 different missing textures so objects can depend on specific ids inside it
	, mMetaLocation(metaFile)
	, mTextureGroup(textures)
	, mResourcePackManager(resourcePackManager)
{

	//the Atlas needs to have a low priority to ensure that it gets the event after the materials are reloaded
	//initListener(10.f);
}

template<typename T>
T& select_min(T& A, T& B) {
	return A > B ? B : A;
}

bool TextureAtlas::_tryAssignUVs(uint32_t width, uint32_t height, std::vector<TextureAtlasTile>& tiles) {
	uint32_t x = getPadSize();
	uint32_t y = getPadSize();
	auto widthFloat = static_cast<float>(width);
	auto heightFloat = static_cast<float>(height);

	uint32_t rowStride = 0;
	for (auto&& tile : tiles) {
		auto aw = tile.getActualWidth();
		auto ah = tile.getActualHeight();
		auto w = tile.getWidth();
		auto h = tile.getHeight();
		
		if (ah > aw) {
			ah = aw;
		}
		if (h > w) {
			h = w;
		}

		uint32_t strideX = w + getPadSize() * 2;
		uint32_t strideY = h + getPadSize() * 2;

		rowStride = std::max(strideY, rowStride);

		if (y + h + getPadSize() > height) {
			return false; //woops won't fit, we failed :(
		}

		TextureUVCoordinateSet uvCoordSet = TextureUVCoordinateSet(
			x / widthFloat,
			y / heightFloat,
			(x + aw) / widthFloat,
			(y + ah) / heightFloat,
			width,
			height,
			tile.location
		);

		DEBUG_ASSERT(uvCoordSet._u0 >= 0.0f && uvCoordSet._u0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v0 >= 0.0f && uvCoordSet._v0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._u1 >= 0.0f && uvCoordSet._u1 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v1 >= 0.0f && uvCoordSet._v1 <= 1.0f, "Invalid UV");

		for (auto& tileUV : tile.uvs) {
			*tileUV = uvCoordSet;
		}

		x += strideX;
		if (x + w + getPadSize() >= width) {
			x = getPadSize();
			y += rowStride;
			rowStride = 0;
		}
	}
	return true;
};

inline int getIndex(int currentWidth, int x, int y) {
	return y * currentWidth * 2 + x;
}

void TextureAtlas::redrawAtlas() {
	
	if (mTextureGroup == nullptr)
		return;

	ResourceLocation rl(mResourceName);

	if (mTextureGroup->isLoaded(rl))
	{
		optional_ref<mce::TexturePair> pair = mTextureGroup->getTexturePair(rl);
		pair.unwrap().clear();
	}
	mce::TextureDescription td;
	td.mHeight = mHeight;
	td.mWidth = mWidth;
	td.mMipMapCount = mMipCount;
	td.mTextureFormat = mce::TextureFormat::R8G8B8A8_UNORM;//0x1c
	td.unk2 = 40;//todo what is this
	mce::TexturePtr local_a0 = mTextureGroup->createEmptyTexture(rl, td);
	for (int i = 0; i < mMipCount; i++)
	{
		_renderAtlasLevel(local_a0,i, mTextureAtlasTiles);
	}
}

void TextureAtlas::_addElement(Json::Value texture, ParsedAtlasNode& newNode, int atIndex, float mipFadeAmount, const Color& mipFadeColor) {
 	if (texture.isObject()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture["path"].asString());
 		auto overlayString = texture["overlay_color"].asString();
 		element.mipFadeAmount = texture["mip_fade_amount"].asFloat(mipFadeAmount);
 		element.mipFadeColor = mipFadeColor;
 		JsonUtil::parseValue(texture["mip_fade_color"], element.mipFadeColor);
 
 		if (overlayString.size() > 0) {
 			element.overlay = Color::fromARGB(strtoul(overlayString.substr(1).c_str(), nullptr, 16));
 			element.overlay.a = 1.f;
		
			newNode.elements[atIndex] = element;
		}
 		
//  		newNode.elements[atIndex] = element;
 	}
 	else if (texture.isString()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture.asString());
 		element.mipFadeAmount = mipFadeAmount;
 		element.mipFadeColor = mipFadeColor;
 		newNode.elements[atIndex] = element;
 	}
 	else {
 		DEBUG_FAIL("INVALID");
 	}
}

void TextureAtlas::_readNode(Json::Value jsonNode, ParsedAtlasNode& node, bool isRefreshing) {
	node.quad = jsonNode["quad"].asBool(false);
	Json::Value texture = jsonNode["textures"];
	float defaultFadeMipAmount = jsonNode["mip_fade_amount"].asFloat();
	Color defaultFadeMipColor = Color::NIL;
	JsonUtil::parseValue(jsonNode["mip_fade_color"], defaultFadeMipColor);

	if (texture.isArray()) {
		if (!isRefreshing) {
			node.elements.resize(texture.size());
		}

		size_t elementIndex = 0;
		for (auto& textureElement : texture) {
			_addElement(textureElement, node, elementIndex, defaultFadeMipAmount, defaultFadeMipColor);
			++elementIndex;
			// This can happen if an override of a vanilla node has more elements in it than we expect
			if (elementIndex >= node.elements.size()) {
				break;
			}
		}
	}
	else {
		if (!isRefreshing) {
			node.elements.resize(1);
		}

		_addElement(texture, node, 0, defaultFadeMipAmount, defaultFadeMipColor);
	}
}

void TextureAtlas::_loadAtlasNodes(Json::Value root, std::vector<ParsedAtlasNode>& atlasNodes) {

	for (auto& nodeName : root.getMemberNames()) {
		ParsedAtlasNode newNode;
		Json::Value jsonNode = root[nodeName];

		newNode.name = nodeName;
		_readNode(jsonNode, newNode, false);

		atlasNodes.push_back(newNode);
	}
}

void TextureAtlas::_addTile(std::vector<TextureAtlasTile> &tiles, TextureUVCoordinateSet* uvSet, bool quad, float fadePerMipAmount, const Color& fadeMipColor, ResourceLocation& location, const Color& overlayColor) {
	bool found = false;
	for (auto& tileInstance : tiles) {
		if (tileInstance.overlay == overlayColor &&
			tileInstance.quad == quad &&
			tileInstance.location == location &&
			tileInstance.mipFadeRate == fadePerMipAmount &&
			tileInstance.mipFadeColor == fadeMipColor) {

			found = true;
			tileInstance.uvs.push_back(uvSet);
		}
	}

	if (!found) {
		TextureAtlasTile tile(overlayColor, quad, location);
		tile.mipFadeRate = fadePerMipAmount;
		tile.mipFadeColor = fadeMipColor;
		tiles.emplace_back(std::move(tile));
		tiles.back().uvs.push_back(uvSet);
	}
}

void TextureAtlas::loadMetaFile() {
	mTextureItems.clear();
	mTextureAtlasTiles.clear();
	mAtlasNodesList.clear();
	//mTextureGroup.clear();

	/* {
		//std::string parseString;
		Json::Value root;
		Json::Reader reader;

		//Resource::load(mMetaLocation, parseString);
		std::string path = "Content/data/resourcepacks/vanilla/resources.json";
		std::ifstream parseString(*path);

		bool parsingSucessful = reader.parse(parseString, root);
		if (!parsingSucessful) {
			return;
		}

		auto textureList = root["resources"]["textures"];
		for (auto& nodeName : textureList.getMemberNames()) {
			mResourceMap[nodeName] = textureList[nodeName].asString();
		}
	}*/

	std::string parseString;
	Json::Value root;
	Json::Reader reader;

	Resource::load(mMetaLocation, parseString);
	//FString path = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/terrain_texture.json";
	//std::ifstream parseString(*path);

	bool parsingSucessful = reader.parse(parseString, root);
	if (!parsingSucessful) {
		return;
	}

	if (mce::RenderDevice::getInstance().checkFeatureSupport(mce::RenderFeature::Mipmaps))
		mMipCount = root.get("num_mip_levels", 1).asInt();
	else
		mMipCount = 1;

	mPadSize = root.get("padding", 0).asInt();

	mResourceName = root["texture_name"].asString();


	/*    auto resourceVersions =
        ResourcePackManager::loadAllVersionsOf(mMetaLocation);

    for (auto const& version : resourceVersions)
    {
        Json::Value versionRoot;

        if (!reader.parse(version, versionRoot))
            continue;

        loadAtlasNodes(versionRoot["texture_data"]);
    }*/


	_loadAtlasNodes(root["texture_data"], mAtlasNodesList);

	// Now that we have the properly overridden nodes,
	// we can use that list to generate the list of tiles
	for (auto& atlasNode : mAtlasNodesList) {
		for (auto& atlasNodeEntry : atlasNode.elements) {
			_addTile(mTextureAtlasTiles, &atlasNodeEntry.uv, atlasNode.quad, atlasNodeEntry.mipFadeAmount, atlasNodeEntry.mipFadeColor, atlasNodeEntry.location, atlasNodeEntry.overlay);
		}
	}

	//TODO how to guess the sizes on the server without loading the textures? Is it even needed? Is any of this needed?
	//for (auto&& tile : mTextureAtlasTiles) {
	//	bool isValid = tile.loadTexture(mTextureGroup, mResourceMap);
	//	if (!isValid) {
	//		UE_LOG(LogTemp, Warning, TEXT("Could not load texture: %s"), UTF8_TO_TCHAR(tile.location.mPath.c_str()));
	//	}
	//	tile.loadTextureEmissive(mTextureGroupEmissive, mResourceMap);
	//}

	if (mTextureGroup)
	{
		for (auto&& tile : mTextureAtlasTiles)
			tile.loadTexture(mTextureGroup);
	}
	//theaperturecat - should this be done? the ue4 version does it but does mcpe 1.0.0 do it?
	/*mTextureAtlasTiles.erase(
		std::remove_if(
			mTextureAtlasTiles.begin(),
			mTextureAtlasTiles.end(),
			[](TextureAtlasTile& element) -> bool {
		return element.texture == nullptr;
	}
		),
		mTextureAtlasTiles.end()
		);*/

	mWidth = mHeight = 1;
	uint32_t area = 0;
	for (auto&& tile : mTextureAtlasTiles) {
		area += (tile.getWidth() + 2 * getPadSize()) * (tile.getHeight() + 2 * getPadSize());
	}

	while (mWidth * mHeight < area) {
		//double the smallest between the dimensions or width
		select_min(mWidth, mHeight) *= 2;
	}

	//assign the uvs to all textures based on their size
	//sort the tiles based on their size, biggest last
	std::stable_sort(mTextureAtlasTiles.begin(), mTextureAtlasTiles.end(), [](const TextureAtlasTile& A, const TextureAtlasTile& B) {
		return A.getArea() < B.getArea();
	});

	//try to fit all tiles inside the assigned sizes, doubling the size and retrying if it fails
	while (!_tryAssignUVs(mWidth, mHeight, mTextureAtlasTiles)) {
		select_min(mWidth, mHeight) *= 2;
	}

	// Now that all the uv's have been generated, we need to build the mTextureItems list
	int nodeIndex = 0;
	for (auto& atlasNode : mAtlasNodesList) {
		TextureAtlasItem newAtlasItem(atlasNode.name, atlasNode.elements.size());
		newAtlasItem.setParsedNodeIndex(nodeIndex);

		int uvIndex = 0;
		for (auto& atlasNodeEntry : atlasNode.elements) {
			newAtlasItem[uvIndex] = atlasNodeEntry.uv;
			++uvIndex;
		}
		mTextureItems[atlasNode.name] = newAtlasItem;
		++nodeIndex;
	}

	redrawAtlas();
}

const TextureAtlasItem& TextureAtlas::getTextureItem(const std::string& textureName) const {
	auto tex = mTextureItems.find(textureName);
	DEBUG_ASSERT(tex != mTextureItems.end(), "This texture item doesn't exist");
	if (tex == mTextureItems.end()) {
		return mMissingTextureItem;
	}
	return tex->second;
}

glm::vec2 TextureAtlas::getAtlasFaceSize() const {
	return mAtlasFaceSize;
}

int TextureAtlas::getPadSize() const {
	return mPadSize;
}

void TextureAtlas::onAppResumed() {
	mce::RenderContextImmediate::get().beginFrame();

	redrawAtlas();

 	mce::RenderContextImmediate::get().endFrame();
}

// glm::vec4 gammaCorrectedAverage(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, const glm::vec4& d) {
// 
// 	// gamma of 2.0
// 	return glm::fastSqrt((a * a + b * b + c * c + d * d) / 4.f);
// }

 void readTexture(glm::u8vec4& color, const glm::vec2& uv, const TextureData& textureData, const uint32_t textureDataStride, const glm::ivec2& maxDimensions) {
 	uint32_t width = textureData.getWidth();
 	uint32_t height = textureData.getHeight();
 
 	if (height > width) {
 		height = width;
 	}
 
 	glm::ivec2 pixelOffset(static_cast<uint32_t>(uv.x * width), static_cast<uint32_t>(uv.y * height));
 
 	// clamping the uv's just like dx/ogl
 	pixelOffset = glm::clamp(pixelOffset, glm::ivec2(0, 0), maxDimensions);
 
 	const uint32_t readPixelIndex = width * pixelOffset.y + pixelOffset.x;
 	const uint32_t readBufferIndex = readPixelIndex * textureDataStride;
 
 	switch (textureData.getTextureFormat()) {
 		case mce::TextureFormat::R8G8B8A8_UNORM:
 		{
 			color = reinterpret_cast<const glm::u8vec4&>(textureData.getDataPacked().at(readBufferIndex));
 			break;
 		}
 		case mce::TextureFormat::R8G8B8_UNORM:
 		{
 			const glm::u8vec3& readColor = reinterpret_cast<const glm::u8vec3&>(textureData.getDataPacked().at(readBufferIndex));
 			color = glm::u8vec4(readColor, std::numeric_limits<glm::u8>::max());
 
 			break;
 		}
 		default:
 			MCEUNKOWNFAIL(textureData.getTextureFormat());
 	}
 }

 void renderSingleTile(const TextureUVCoordinateSet& uvs, TextureData& atlasTexture, const TextureData& textureData, int padSize, const Color& overlay, const bool quad, const uint32_t mipID, float mipFadeAmount, Color mipFadeColor) {
 	const float toUINT = std::numeric_limits<glm::u8>::max();
 	const float toFloat = 1.f / toUINT;
 
 	const glm::vec4 CURRENT_COLOR(overlay.r, overlay.g, overlay.b, overlay.a);
 
 	uint32_t width = textureData.getWidth();
 	uint32_t height = textureData.getHeight();
 
 	if (height > width) {
 		height = width;
 	}
 
 	const glm::ivec2 maxDimensions(width - 1, height - 1);
 
 	const int32_t blurRadius = 1 << mipID;
 	const float inverseBlurRadius = 1.f / static_cast<float>(blurRadius);
 	const float inverseBlurRadiusSquared = inverseBlurRadius * inverseBlurRadius;
 
 	// this using not modified pad size because it using texture size wich never change based on mipmap
 	float uvPadWidth = (padSize / static_cast<float>(uvs._texSizeW));
 	float uvPadHeight = (padSize / static_cast<float>(uvs._texSizeH));
 
 	//actual pad should scale down based on mipmap
 	padSize >>= mipID;
 
 	float quadScale = quad ? 2.0f : 1.0f;
 	float x0 = uvs._u0 - uvPadWidth;
 	float y0 = uvs._v0 - uvPadHeight;
 	float w = (uvs._u1 - uvs._u0) * quadScale + uvPadWidth * 2;
 	float h = (uvs._v1 - uvs._v0) * quadScale + uvPadHeight * 2;
 	float x1 = x0 + w;
 	float y1 = y0 + h;
 
 
 	// x pixel coordinate start/end
 	const uint32_t xp0 = static_cast<uint32_t>(x0 *  atlasTexture.getWidth());
 	const uint32_t xp1 = static_cast<uint32_t>(x1 *  atlasTexture.getWidth());
 
 	// y pixel coordinate start/end
 	const uint32_t yp0 = static_cast<uint32_t>(y0 *  atlasTexture.getHeight());
 	const uint32_t yp1 = static_cast<uint32_t>(y1 *  atlasTexture.getHeight());
 
 	const uint32_t writeStride = mce::TextureHelper::textureFormatToByteStride(atlasTexture.getTextureFormat());
 	const uint32_t readStride = mce::TextureHelper::textureFormatToByteStride(textureData.getTextureFormat());
 
 	const glm::vec2 inverseReadTextureDimensions = glm::vec2(1.0f / (xp1 - xp0), 1.f / (yp1 - yp0));
 	const glm::vec2 mipmappedTextureSize = glm::vec2(width >> mipID, height >> mipID);
 
 
 	std::vector<uint8_t>& atlasBuffer = atlasTexture.getDataPackedModify();
 
 	for (uint32_t yPixelIndex = yp0; yPixelIndex != yp1; ++yPixelIndex) {
 
 		for (uint32_t xPixelIndex = xp0; xPixelIndex != xp1; ++xPixelIndex) {
 
 			// atlas
 			const uint32_t atlasPixelIndex = atlasTexture.getWidth() * yPixelIndex + xPixelIndex;
 			const uint32_t atlasBufferIndex = atlasPixelIndex * writeStride;
 
 			MCEASSERT(atlasBufferIndex < atlasBuffer.size());
 
 			// get the location of where we want to write in the atlas
 			uint8_t* const colorPointer = &atlasBuffer.at(atlasBufferIndex);
 			glm::u8vec4& atlasColor = *reinterpret_cast<glm::u8vec4*>(colorPointer);
 
 			// calculate uv's
 			//0 to 1 for actual data padding will be 0 or 1
 			glm::vec2 uv((float(xPixelIndex) - xp0 - padSize) / mipmappedTextureSize.x, (float(yPixelIndex) - yp0 - padSize) / mipmappedTextureSize.y);
 
 			// run our "shader code" to apply the color, convert to float
 			glm::vec4 atlasColorFloat;
 
 			if (blurRadius <= 1) {
 
 				// finally read from the raw bytes into our buffer, this assumes RGBA8_UNORM for dest
 				readTexture(atlasColor, uv, textureData, readStride, maxDimensions);
 
 				atlasColorFloat = glm::vec4(atlasColor.r * toFloat, atlasColor.g * toFloat, atlasColor.b * toFloat, atlasColor.a * toFloat);
 			}
 			else {
 
 				glm::vec4 filterColor;
 				glm::u8vec4 filterTextureColor;
 				glm::vec2 blurUV;
 				for (int32_t i = 0; i < blurRadius; ++i) {
 					for (int32_t j = 0; j < blurRadius; ++j) {
 						blurUV = uv + inverseReadTextureDimensions * glm::vec2(j, i);
 
 						readTexture(filterTextureColor, blurUV, textureData, readStride, maxDimensions);
 						filterColor += glm::vec4(filterTextureColor) * toFloat;
 					}
 				}
 
 				filterColor *= inverseBlurRadiusSquared;
 
 				atlasColorFloat = filterColor;
 			}
 
 			const glm::vec4::value_type alpha = atlasColorFloat.a;
 
 			atlasColorFloat = glm::mix(atlasColorFloat, CURRENT_COLOR * atlasColorFloat, alpha * CURRENT_COLOR.a);
 
 			atlasColorFloat.a = alpha;
 
 			if (CURRENT_COLOR.a > 0) {
 				atlasColorFloat.a = 1.f;
 			}
 
 			if (mipFadeAmount > 0.0f) {
 				Color asColor(atlasColorFloat.r, atlasColorFloat.g, atlasColorFloat.b, atlasColorFloat.a);
 				asColor = Color::lerp(asColor, mipFadeColor, mipFadeAmount);
 				atlasColorFloat.r = asColor.r;
 				atlasColorFloat.g = asColor.g;
 				atlasColorFloat.b = asColor.b;
 				atlasColorFloat.a = asColor.a;
 			}
 
 			// convert back to 8 bit UNORM
 			atlasColorFloat *= toUINT;
 
 			// truncate and assign
 			atlasColor = glm::u8vec4(atlasColorFloat.r, atlasColorFloat.g, atlasColorFloat.b, atlasColorFloat.a);
 
 		}
 	}
}

//original mcpe 0.15.0 hardware-rendered version, didnt work with resource packs well
/*void renderSingleTile(
	const TextureUVCoordinateSet& uvs,
	mce::TexturePtr& texture,
	int padSize,
	const Color& overlay,
	bool quad,
	mce::MaterialPtr& material)
{
#ifdef TAC_COMPILE
	currentShaderColor.setColor(overlay);
#endif
	const float padU = static_cast<float>(padSize) /
		static_cast<float>(uvs._texSizeW);

	const float padV = static_cast<float>(padSize) /
		static_cast<float>(uvs._texSizeH);

	auto* desc = texture->getDescription();

	const float depth =
		static_cast<float>(padSize) /
		static_cast<float>(desc->mWidth);

	const float scale = quad ? 2.0f : 1.0f;

	const float left = uvs._u0 - padU;
	const float top = uvs._v0 - padV;
	const float right = (uvs._u1 - uvs._u0) * scale + uvs._u0 + padU;
	const float bottom = (uvs._v1 - uvs._v0) * scale + uvs._v0 + padV;

	const float x0 = left * 2.0f - 1.0f;
	const float y0 = top * 2.0f - 1.0f;
	const float x1 = right * 2.0f - 1.0f;
	const float y1 = bottom * 2.0f - 1.0f;

	if (quad)
	{
		Tessellator::instance.begin(16);

		const float midX = (x0 + x1) * 0.5f;
		const float midY = (y0 + y1) * 0.5f;

		const float inv = 1.0f / (depth * 2.0f + 1.0f);
		const float inner = depth * inv;
		const float outer = (depth + 1.0f) * inv;

		Tessellator::instance.vertexUV(x0, midY, depth, 0.0f, outer);
		Tessellator::instance.vertexUV(midX, midY, depth, outer, outer);
		Tessellator::instance.vertexUV(midX, y1, depth, outer, 1.0f);
		Tessellator::instance.vertexUV(x0, y1, depth, 0.0f, 1.0f);

		Tessellator::instance.vertexUV(midX, midY, depth, inner, outer);
		Tessellator::instance.vertexUV(x1, midY, depth, 1.0f, outer);
		Tessellator::instance.vertexUV(x1, y1, depth, 1.0f, 1.0f);
		Tessellator::instance.vertexUV(midX, y1, depth, inner, 1.0f);

		Tessellator::instance.vertexUV(x0, y0, depth, 0.0f, 0.0f);
		Tessellator::instance.vertexUV(midX, y0, depth, outer, 0.0f);
		Tessellator::instance.vertexUV(midX, midY, depth, outer, inner);
		Tessellator::instance.vertexUV(x0, midY, depth, 0.0f, inner);

		Tessellator::instance.vertexUV(midX, y0, depth, inner, 0.0f);
		Tessellator::instance.vertexUV(x1, y0, depth, 1.0f, 0.0f);
		Tessellator::instance.vertexUV(x1, midY, depth, 1.0f, inner);
		Tessellator::instance.vertexUV(midX, midY, depth, inner, inner);
	}
	else
	{
		Tessellator::instance.begin(4);

		Tessellator::instance.vertexUV(x0, y0, depth, 0.0f, 0.0f);
		Tessellator::instance.vertexUV(x1, y0, depth, 1.0f, 0.0f);
		Tessellator::instance.vertexUV(x1, y1, depth, 1.0f, 1.0f);
		Tessellator::instance.vertexUV(x0, y1, depth, 0.0f, 1.0f);
	}

	Tessellator::instance.draw(material, texture);
}*/

void TextureAtlas::_renderAtlasLevel(mce::TexturePtr& atlasTexture, const uint32_t mipID, const std::vector<TextureAtlasTile>& tiles) {
//void TextureAtlas::_renderAtlasLevel(uint32_t mipID, std::vector<TextureAtlasTile>& tiles) {
	/*TextureAtlasTile* dirtTile = nullptr;
	std::vector<TextureAtlasTile*> grassSides;
	for (auto& tile : tiles) {
		if (tile.location.mPath == "block.dirt") {
			dirtTile = &tile;
		}
		else if (tile.location.mPath == "block.grass.side") {
			grassSides.emplace_back(&tile);
		}
	}*/

	/*FTexture2DMipMap& AtlasMip = atlasTexture->PlatformData->Mips[0];
	uint8* atlasData = (uint8*)AtlasMip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasData, 0, atlasTexture->GetSizeX() * atlasTexture->GetSizeY() * 4);

	FTexture2DMipMap& AtlasMipEmissive = atlasTextureEmissive->PlatformData->Mips[0];
	uint8* atlasDataEmissive = (uint8*)AtlasMipEmissive.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasDataEmissive, 0, atlasTextureEmissive->GetSizeX() * atlasTextureEmissive->GetSizeY() * 4);

	if (dirtTile != nullptr) {
		for (auto* tile : grassSides) {
			renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile->uvs[0], dirtTile->texture, getPadSize(), dirtTile->overlay, dirtTile->quad);
		}
	}

	for (auto& tile : tiles) {
		renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile.uvs[0], tile.texture, getPadSize(), tile.overlay, tile.quad);

		if (tile.textureEmissive != nullptr) {
			renderSingleTile(atlasDataEmissive, atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad);
		}
	}

	AtlasMipEmissive.BulkData.Unlock();
	AtlasMip.BulkData.Unlock();*/

	 	auto& renderContext = mce::RenderContextImmediate::get();
	 
	 	mce::TextureDescription atlasDescription = atlasTexture->getDescription();
	 
	 	atlasDescription.mWidth >>= mipID;
		atlasDescription.mHeight >>= mipID;
	 
	 	TextureData atlasTextureData(atlasDescription);
	 
	 	atlasTextureData.getDataPackedModify().resize(atlasTexture->getDescription().getSizeInBytes());
	 
	 	for (auto& tile : tiles) {
	 		renderSingleTile(*tile.uvs[0], atlasTextureData, *tile.mTextureData, getPadSize(), tile.overlay, tile.quad, mipID, tile.mipFadeRate * mipID, tile.mipFadeColor);
	 	}
	 
	 	atlasTexture->subBuffer(renderContext, atlasTextureData.getDataPackedModify().data(), 0, 0, atlasTextureData.getWidth(), atlasTextureData.getHeight(), mipID);
}

// #else //#ifndef MCPE_HEADLESS
// 
// TextureAtlas::TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures)
// 	: AppPlatformListener(false)
// 	, mMetaLocation(metaFile)
// 	, mTextureGroup(textures)
// 	, mResourcePackManager(resourcePackManager)
// 	, mMissingTextureItem("", 255) // HACK: 255 different missing textures so objects can depend on specific ids inside it
// {
// 
// }
// 
// void TextureAtlas::loadMetaFile() {
// 
// }
// 
// void TextureAtlas::onAppResumed() {
// 
// }
// 
// const TextureAtlasItem& TextureAtlas::getTextureItem(const std::string& textureName) const {
// 	return mMissingTextureItem;
// }
// 
// void TextureAtlas::redrawAtlas() {
// 
// }
// 
// #endif //#ifndef MCPE_HEADLESS