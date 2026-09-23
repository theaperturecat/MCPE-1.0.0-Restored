/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CameraBlock.h"

#include "world/Facing.h"
#include "world/level/material/Material.h"
#ifdef TAC_COMPILE
#include "client/renderer/texture/TextureAtlasItem.h"
#endif


CameraBlock::CameraBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {

	mProperties = BlockProperty::Unspecified;
	mRenderLayer = RENDERLAYER_OPAQUE;
}
