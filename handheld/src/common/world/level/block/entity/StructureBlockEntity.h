#pragma once

#include "BlockEntity.h"
class StructureBlockEntity : public BlockEntity
{
public:
	static constexpr char* STRUCTURE_ASSET_FILE_PREFIX = "structures/";
	static constexpr char* STRUCTURE_FILE_POSTFIX = ".nbt";
};