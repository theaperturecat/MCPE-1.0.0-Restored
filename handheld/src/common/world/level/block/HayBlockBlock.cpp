/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "common_header.h"

#include "world/level/block/HayBlockBlock.h"
#include "world/level/material/Material.h"


HayBlockBlock::HayBlockBlock(const std::string& nameId, int id) :
	RotatedPillarBlock(nameId, id, Material::getMaterial(MaterialType::Dirt)) {
}
