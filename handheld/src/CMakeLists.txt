cmake_minimum_required(VERSION 3.25)

project(minecraftpe)

option(ISPUB "Is public build?" 0)
option(DEDICATED_SERVER "Is dedicated server?" 0)#Really all this means now is whether to compile client/renderer lol
set(COMPILE_PLATFORM "WINDOWS" CACHE STRING "Target PLATFORM (WINDOWS/ANDROID/IOS)")
set(USEGUI 1)

#Nice function for adding all the directories to the project
function(group_files_by_directory root_dir ext group_name)
    file(GLOB_RECURSE files "${root_dir}/*.${ext}")
    message(STATUS ${root_dir})
    foreach(file ${files})
        get_filename_component(dir ${file} DIRECTORY)
        file(RELATIVE_PATH relative_dir ${CMAKE_CURRENT_SOURCE_DIR} ${dir})
        message(STATUS ${file})
        source_group("${group_name}/${relative_dir}" FILES ${file})
    endforeach()
endfunction()

group_files_by_directory("common" "cpp" "Source files")
group_files_by_directory("common" "c" "Source files")
group_files_by_directory("common" "h" "Header files")

group_files_by_directory("win" "cpp" "Source files")
group_files_by_directory("win" "c" "Source files")
group_files_by_directory("win" "h" "Header files")

#if(COMPILE_PLATFORM STREQUAL "ANDROID")
#add_library(minecraftpe SHARED)
#else()
#add_library(minecraftpe STATIC)
add_executable(minecraftpe)
#endif()

target_precompile_headers(minecraftpe PRIVATE common/pch.h)

target_compile_features(minecraftpe PRIVATE cxx_std_17)

target_include_directories(minecraftpe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/common ${CMAKE_CURRENT_SOURCE_DIR}/common/client)

if(ISPUB)
target_compile_definitions(minecraftpe
    PRIVATE NDEBUG=1 PUBLISH=1
)
endif()



if(DEDICATED_SERVER)

target_compile_definitions(minecraftpe
    PRIVATE STANDALONE_SERVER=1 MCPE_HEADLESS=1
)



set(USEGUI 0)
endif()




if(COMPILE_PLATFORM STREQUAL "WINDOWS")

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT minecraftpe)

set_property(TARGET minecraftpe PROPERTY 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../data/"
)

target_compile_definitions(minecraftpe
PRIVATE NOMINMAX=1)

target_link_libraries(minecraftpe PUBLIC 
ws2_32
kernel32
user32
gdi32
winspool
comdlg32
advapi32
shell32
ole32
oleaut32
uuid
odbc32
odbccp32
)

target_sources(minecraftpe
PRIVATE
../src-client/win/renderer.cpp
../src-client/win/shader.cpp
../src-client/win/main.cpp
PUBLIC
../src-client/win/shader.h
../src-client/win/main_win32.h
)

if(USEGUI)

target_sources(minecraftpe
PRIVATE
#../src-client/win/main.cpp
#win/AppPlatform_win32.cpp
PUBLIC
#win/AppPlatform_win32.h
#../src-client/win/main_win32.h
)
endif()
endif()

target_sources(minecraftpe
PRIVATE
#common/app.cpp
#common/applifecyclecontext.cpp
#common/appplatformlistener.cpp
common/CommonTypes.cpp
#common/gamecontrollerhandler_windows.cpp
#common/pch.cpp
common/SharedConstants.cpp
common/GameVersion.cpp

common/locale/I18n.cpp

common/nbt/ByteArrayTag.cpp
common/nbt/ByteTag.cpp
common/nbt/CompoundTag.cpp
common/nbt/DoubleTag.cpp
common/nbt/FloatTag.cpp
common/nbt/Int64Tag.cpp
common/nbt/IntArrayTag.cpp
common/nbt/IntTag.cpp
common/nbt/ListTag.cpp
common/nbt/ShortTag.cpp
common/nbt/StringTag.cpp
common/nbt/Tag.cpp

common/network/BinaryStream.cpp

common/platform/file.cpp
#common/platform/threading/workerpool.cpp
#common/platform/threading/semaphore.cpp
#common/platform/threading/backgroundworker.cpp
#common/platform/oculusplatformmessagepump.cpp
#common/platform/debugeventlogger.cpp
#common/platform/appplatform.cpp


common/util/Base64.cpp
common/util/BlockListSerializer.cpp
common/util/ClassID.cpp
common/util/ColorFormat.cpp
common/util/DataIO.cpp
common/util/DebugStats.cpp
common/util/Easing.cpp
common/util/FilePickerSettings.cpp
common/util/FloatRange.cpp
common/util/GzipUtils.cpp
common/util/ItemListSerializer.cpp
common/util/JSONReadVerifier.cpp
common/util/JSONUtils.cpp
common/util/LootTableUtil.cpp
common/util/Math.cpp
common/util/PoolAllocator.cpp
common/util/SemVersion.cpp
common/util/SharedLock.cpp
common/util/StringUtils.cpp
common/util/Token.cpp
common/util/VariantParameterList.cpp
common/util/PerfTimer.cpp

common/world/Container.cpp
common/world/Direction.cpp
common/world/Facing.cpp
common/world/attribute/Attribute.cpp
common/world/entity/Entity.cpp
common/world/entity/EntityClassTree.cpp
common/world/entity/EntityDefinitionIdentifier.cpp
common/world/entity/EntityTypes.cpp
common/world/entity/Mob.cpp
common/world/entity/player/Abilities.cpp
common/world/entity/player/Player.cpp
common/world/item/Item.cpp
common/world/item/ItemCategory.cpp
common/world/item/ItemInstance.cpp
common/world/level/BlockPatternBuilder.cpp
common/world/level/BlockPos.cpp
common/world/level/BlockSource.cpp
common/world/level/BlockSourceListener.cpp
common/world/level/ChunkBlockPos.cpp
common/world/level/ChunkPos.cpp
common/world/level/FoliageColor.cpp
common/world/level/Level.cpp
common/world/level/LevelSettings.cpp
common/world/level/LightLayer.cpp
common/world/level/Seasons.cpp
common/world/level/Tick.cpp
common/world/level/Weather.cpp
common/world/level/biome/BeachBiome.cpp
common/world/level/biome/Biome.cpp
common/world/level/biome/BiomeDecorator.cpp
common/world/level/biome/BiomeSource.cpp
common/world/level/biome/DesertBiome.cpp
common/world/level/biome/ExtremeHillsBiome.cpp
common/world/level/biome/FixedBiomeSource.cpp
common/world/level/biome/ForestBiome.cpp
common/world/level/biome/HellBiome.cpp
common/world/level/biome/IceBiome.cpp
common/world/level/biome/JungleBiome.cpp
common/world/level/biome/MesaBiome.cpp
common/world/level/biome/MobSpawnUtils.cpp
common/world/level/biome/MushroomIslandBiome.cpp
common/world/level/biome/MutatedBiome.cpp
common/world/level/biome/MutatedBirchForest.cpp
common/world/level/biome/MutatedSavannaBiome.cpp
common/world/level/biome/PlainsBiome.cpp
common/world/level/biome/SavannaBiome.cpp
common/world/level/biome/StoneBeachBiome.cpp
common/world/level/biome/SwampBiome.cpp
common/world/level/biome/TaigaBiome.cpp
common/world/level/biome/TheEndBiome.cpp
common/world/level/biome/TheEndBiomeDecorator.cpp
common/world/level/block/ActivatorRailBlock.cpp
common/world/level/block/AirBlock.cpp
common/world/level/block/AnvilBlock.cpp
common/world/level/block/BasePressurePlateBlock.cpp
common/world/level/block/BaseRailBlock.cpp
common/world/level/block/BeaconBlock.cpp
common/world/level/block/BedBlock.cpp
common/world/level/block/BedrockBlock.cpp
common/world/level/block/BeetrootBlock.cpp
common/world/level/block/Block.cpp
common/world/level/block/BlockState.cpp
common/world/level/block/BookshelfBlock.cpp
common/world/level/block/BorderBlock.cpp
common/world/level/block/BrewingStandBlock.cpp
common/world/level/block/BushBlock.cpp
common/world/level/block/ButtonBlock.cpp
common/world/level/block/CactusBlock.cpp
common/world/level/block/CakeBlock.cpp
common/world/level/block/CameraBlock.cpp
common/world/level/block/CarrotBlock.cpp
common/world/level/block/CauldronBlock.cpp
common/world/level/block/ChalkboardBlock.cpp
common/world/level/block/ChestBlock.cpp
common/world/level/block/ChorusFlowerBlock.cpp
common/world/level/block/ChorusPlantBlock.cpp
common/world/level/block/ClayBlock.cpp
common/world/level/block/ClothBlock.cpp
common/world/level/block/CocoaBlock.cpp
common/world/level/block/ColoredBlock.cpp
common/world/level/block/ComparatorBlock.cpp
common/world/level/block/CropBlock.cpp
common/world/level/block/DaylightDetectorBlock.cpp
common/world/level/block/DeadBush.cpp
common/world/level/block/DetectorRailBlock.cpp
common/world/level/block/DiodeBlock.cpp
common/world/level/block/DirtBlock.cpp
common/world/level/block/DispenserBlock.cpp
common/world/level/block/DoorBlock.cpp
common/world/level/block/DoublePlantBlock.cpp
common/world/level/block/DragonEggBlock.cpp
common/world/level/block/DropperBlock.cpp
common/world/level/block/EnchantingTableBlock.cpp
common/world/level/block/EnderChestBlock.cpp
common/world/level/block/EndGatewayBlock.cpp
common/world/level/block/EndPortalBlock.cpp
common/world/level/block/EndPortalFrameBlock.cpp
common/world/level/block/EndRodBlock.cpp
common/world/level/block/EntityBlock.cpp
common/world/level/block/FarmBlock.cpp
common/world/level/block/FenceBlock.cpp
common/world/level/block/FenceGateBlock.cpp
common/world/level/block/FireBlock.cpp
common/world/level/block/FlowerBlock.cpp
common/world/level/block/FlowerPotBlock.cpp
common/world/level/block/FurnaceBlock.cpp
common/world/level/block/GlassBlock.cpp
common/world/level/block/GrassBlock.cpp
common/world/level/block/GrassPathBlock.cpp
common/world/level/block/GravelBlock.cpp
common/world/level/block/HalfTransparentBlock.cpp
common/world/level/block/HayBlockBlock.cpp
common/world/level/block/HeavyBlock.cpp
common/world/level/block/HopperBlock.cpp
common/world/level/block/HugeMushroomBlock.cpp
common/world/level/block/IceBlock.cpp
common/world/level/block/InvisibleBlock.cpp
common/world/level/block/ItemFrameBlock.cpp
common/world/level/block/LadderBlock.cpp
common/world/level/block/LeafBlock.cpp
common/world/level/block/LevelEvent.cpp
common/world/level/block/LeverBlock.cpp
common/world/level/block/LightGemBlock.cpp
common/world/level/block/LiquidBlock.cpp
common/world/level/block/LiquidBlockDynamic.cpp
common/world/level/block/LiquidBlockStatic.cpp
common/world/level/block/LogBlock.cpp
common/world/level/block/MelonBlock.cpp
common/world/level/block/MetalBlock.cpp
common/world/level/block/MobSpawnerBlock.cpp
common/world/level/block/MonsterEggBlock.cpp
common/world/level/block/MultiTextureBlock.cpp
common/world/level/block/MushroomBlock.cpp
common/world/level/block/MyceliumBlock.cpp
common/world/level/block/NetherReactor.cpp
common/world/level/block/NetherReactorPattern.cpp
common/world/level/block/NetherWartBlock.cpp
common/world/level/block/NewLeafBlock.cpp
common/world/level/block/NewLogBlock.cpp
common/world/level/block/NoteBlock.cpp
common/world/level/block/ObserverBlock.cpp
common/world/level/block/ObsidianBlock.cpp
common/world/level/block/OldLeafBlock.cpp
common/world/level/block/OldLogBlock.cpp
common/world/level/block/OreBlock.cpp
common/world/level/block/PistonArmBlock.cpp
common/world/level/block/PistonBlock.cpp
common/world/level/block/PodzolBlock.cpp
common/world/level/block/PortalBlock.cpp
common/world/level/block/PotatoBlock.cpp
common/world/level/block/PoweredRailBlock.cpp
common/world/level/block/PressurePlateBlock.cpp
common/world/level/block/PrismarineBlock.cpp
common/world/level/block/PumpkinBlock.cpp
common/world/level/block/QuartzBlockBlock.cpp
common/world/level/block/RailBlock.cpp
common/world/level/block/RedstoneBlock.cpp
common/world/level/block/RedstoneLampBlock.cpp
common/world/level/block/RedStoneOreBlock.cpp
common/world/level/block/RedstoneTorchBlock.cpp
common/world/level/block/RedStoneWireBlock.cpp
common/world/level/block/ReedBlock.cpp
common/world/level/block/RepeaterBlock.cpp
common/world/level/block/RotatedPillarBlock.cpp
common/world/level/block/SandBlock.cpp
common/world/level/block/SandStoneBlock.cpp
common/world/level/block/Sapling.cpp
common/world/level/block/SeaLanternBlock.cpp
common/world/level/block/SignBlock.cpp
common/world/level/block/SkullBlock.cpp
common/world/level/block/SlabBlock.cpp
common/world/level/block/SlimeBlock.cpp
common/world/level/block/SnowBlock.cpp
common/world/level/block/SoulSandBlock.cpp
common/world/level/block/SpongeBlock.cpp
common/world/level/block/StainedGlassBlock.cpp
common/world/level/block/StainedGlassPaneBlock.cpp
common/world/level/block/StairBlock.cpp
common/world/level/block/StemBlock.cpp
common/world/level/block/StoneBlock.cpp
common/world/level/block/StoneButtonBlock.cpp
common/world/level/block/StonecutterBlock.cpp
common/world/level/block/StoneSlabBlock.cpp
common/world/level/block/StructureBlock.cpp
common/world/level/block/StructureVoid.cpp
common/world/level/block/TallGrass.cpp
common/world/level/block/ThinFenceBlock.cpp
common/world/level/block/TntBlock.cpp
common/world/level/block/TopSnowBlock.cpp
common/world/level/block/TorchBlock.cpp
common/world/level/block/TransparentBlock.cpp
common/world/level/block/TrapDoorBlock.cpp
common/world/level/block/TripWireBlock.cpp
common/world/level/block/TripWireHookBlock.cpp
common/world/level/block/VineBlock.cpp
common/world/level/block/WallBlock.cpp
common/world/level/block/WaterlilyBlock.cpp
common/world/level/block/WebBlock.cpp
common/world/level/block/WeightedPressurePlateBlock.cpp
common/world/level/block/WoodBlock.cpp
common/world/level/block/WoodButtonBlock.cpp
common/world/level/block/WoodSlabBlock.cpp
common/world/level/block/WoolCarpetBlock.cpp
common/world/level/block/WorkbenchBlock.cpp
common/world/level/block/entity/BlockEntity.cpp
common/world/level/chunk/ChunkRefCount.cpp
common/world/level/chunk/ChunkSource.cpp
common/world/level/chunk/ChunkViewSource.cpp
common/world/level/chunk/EmptyChunkSource.cpp
common/world/level/chunk/LevelChunk.cpp
common/world/level/chunk/MainChunkSource.cpp
common/world/level/chunk/NetworkChunkSource.cpp
common/world/level/chunk/PlayerChunkSource.cpp
common/world/level/chunk/PostprocessingManager.cpp
common/world/level/chunk/WorldLimitChunkSource.cpp
common/world/level/dimension/Dimension.cpp
common/world/level/dimension/HellDimension.cpp
common/world/level/dimension/NormalDimension.cpp
common/world/level/dimension/end/TheEndDimension.cpp
common/world/level/levelgen/feature/BirchFeature.cpp
common/world/level/levelgen/feature/BlockBlobFeature.cpp
common/world/level/levelgen/feature/CactusFeature.cpp
common/world/level/levelgen/feature/ClayFeature.cpp
common/world/level/levelgen/feature/DeadBushFeature.cpp
common/world/level/levelgen/feature/DesertWellFeature.cpp
common/world/level/levelgen/feature/DoublePlantFeature.cpp
common/world/level/levelgen/feature/EndGatewayFeature.cpp
common/world/level/levelgen/feature/EndIslandFeature.cpp
common/world/level/levelgen/feature/EndPodiumFeature.cpp
common/world/level/levelgen/feature/FancyTreeFeature.cpp
common/world/level/levelgen/feature/Feature.cpp
common/world/level/levelgen/feature/FlowerFeature.cpp
common/world/level/levelgen/feature/GlowStoneFeature.cpp
common/world/level/levelgen/feature/GroundBushFeature.cpp
common/world/level/levelgen/feature/HellFireFeature.cpp
common/world/level/levelgen/feature/HellSpringFeature.cpp
common/world/level/levelgen/feature/HugeMushroomFeature.cpp
common/world/level/levelgen/feature/IcePatchFeature.cpp
common/world/level/levelgen/feature/IceSpikeFeature.cpp
common/world/level/levelgen/feature/JungleTreeFeature.cpp
common/world/level/levelgen/feature/LakeFeature.cpp
common/world/level/levelgen/feature/MegaJungleTreeFeature.cpp
common/world/level/levelgen/feature/MegaPineTreeFeature.cpp
common/world/level/levelgen/feature/MegaTreeFeature.cpp
common/world/level/levelgen/feature/MelonFeature.cpp
common/world/level/levelgen/feature/MonsterRoomFeature.cpp
common/world/level/levelgen/feature/OreFeature.cpp
common/world/level/levelgen/feature/PineFeature.cpp
common/world/level/levelgen/feature/PumpkinFeature.cpp
common/world/level/levelgen/feature/ReedsFeature.cpp
common/world/level/levelgen/feature/RoofTreeFeature.cpp
common/world/level/levelgen/feature/SandFeature.cpp
common/world/level/levelgen/feature/SavannaTreeFeature.cpp
common/world/level/levelgen/feature/SpikeFeature.cpp
common/world/level/levelgen/feature/SpringFeature.cpp
common/world/level/levelgen/feature/SpruceFeature.cpp
common/world/level/levelgen/feature/SwampTreeFeature.cpp
common/world/level/levelgen/feature/TallGrassFeature.cpp
common/world/level/levelgen/feature/TreeFeature.cpp
common/world/level/levelgen/feature/VinesFeature.cpp
common/world/level/levelgen/feature/WaterlilyFeature.cpp
common/world/level/levelgen/flat/FlatLevelSource.cpp
common/world/level/levelgen/structure/BoundingBox.cpp
common/world/level/levelgen/structure/EndCityFeature.cpp
common/world/level/levelgen/structure/EndCityPieces.cpp
common/world/level/levelgen/structure/MineshaftFeature.cpp
common/world/level/levelgen/structure/MineshaftPieces.cpp
common/world/level/levelgen/structure/NetherBridgeFeature.cpp
common/world/level/levelgen/structure/NetherBridgePieces.cpp
common/world/level/levelgen/structure/OceanMonumentFeature.cpp
common/world/level/levelgen/structure/OceanMonumentPieces.cpp
common/world/level/levelgen/structure/RandomScatteredLargeFeature.cpp
common/world/level/levelgen/structure/ScatteredFeaturePieces.cpp
common/world/level/levelgen/structure/StrongholdFeature.cpp
common/world/level/levelgen/structure/StrongholdPieces.cpp
common/world/level/levelgen/structure/StructureFeature.cpp
common/world/level/levelgen/structure/StructureManager.cpp
common/world/level/levelgen/structure/StructurePiece.cpp
common/world/level/levelgen/structure/StructurePieceFactory.cpp
common/world/level/levelgen/structure/StructureStart.cpp
common/world/level/levelgen/structure/StructureTemplate.cpp
common/world/level/levelgen/structure/StructureTemplateHelpers.cpp
common/world/level/levelgen/structure/StructureTemplateMapping.cpp
common/world/level/levelgen/structure/TemplateStructurePiece.cpp
common/world/level/levelgen/structure/VillageFeature.cpp
common/world/level/levelgen/structure/VillagePieces.cpp
common/world/level/levelgen/synth/ImprovedNoise.cpp
common/world/level/levelgen/synth/PerlinNoise.cpp
common/world/level/levelgen/synth/PerlinSimplexNoise.cpp
common/world/level/levelgen/synth/SimplexNoise.cpp
common/world/level/levelgen/v1/EndGatewayChunkSource.cpp
common/world/level/levelgen/v1/FarlandsFeature.cpp
common/world/level/levelgen/v1/HellRandomLevelSource.cpp
common/world/level/levelgen/v1/LargeCaveFeature.cpp
common/world/level/levelgen/v1/LargeFeature.cpp
common/world/level/levelgen/v1/LargeHellCaveFeature.cpp
common/world/level/levelgen/v1/RandomLevelSource.cpp
common/world/level/levelgen/v1/TheEndRandomLevelSource.cpp
common/world/level/material/Material.cpp
common/world/level/newbiome/AddDeepOceanLayer.cpp
common/world/level/newbiome/AddEdgeLayer.cpp
common/world/level/newbiome/AddIslandLayer.cpp
common/world/level/newbiome/AddMushroomIslandLayer.cpp
common/world/level/newbiome/AddSnowLayer.cpp
common/world/level/newbiome/BiomeEdgeLayer.cpp
common/world/level/newbiome/BiomeInitLayer.cpp
common/world/level/newbiome/DownfallLayer.cpp
common/world/level/newbiome/DownfallMixerLayer.cpp
common/world/level/newbiome/FuzzyZoomLayer.cpp
common/world/level/newbiome/IslandLayer.cpp
common/world/level/newbiome/Layer.cpp
common/world/level/newbiome/RareBiomeSpotLayer.cpp
common/world/level/newbiome/RegionHillsLayer.cpp
common/world/level/newbiome/RemoveTooMuchOceanLayer.cpp
common/world/level/newbiome/RiverInitLayer.cpp
common/world/level/newbiome/RiverLayer.cpp
common/world/level/newbiome/RiverMixerLayer.cpp
common/world/level/newbiome/ShoreLayer.cpp
common/world/level/newbiome/SmoothLayer.cpp
common/world/level/newbiome/SmoothZoomLayer.cpp
common/world/level/newbiome/VoronoiZoom.cpp
common/world/level/newbiome/ZoomLayer.cpp
common/world/level/pathfinder/BinaryHeap.cpp
common/world/level/pathfinder/Node.cpp
common/world/level/pathfinder/Path.cpp
common/world/level/pathfinder/PathFinder.cpp
common/world/level/storage/DBChunkStorage.cpp
common/world/level/storage/DBStorage.cpp
common/world/level/storage/ExternalFileLevelStorage.cpp
common/world/level/storage/ExternalFileLevelStorageSource.cpp
common/world/level/storage/GameRules.cpp
common/world/level/storage/LevelData.cpp
common/world/level/storage/LevelStorage.cpp
common/world/level/storage/LevelSummary.cpp
common/world/level/storage/RegionFile.cpp
common/world/level/storage/SavedDataStorage.cpp
common/world/phys/AABB.cpp
common/world/phys/HitResult.cpp
common/world/phys/Vec2.cpp
common/world/phys/Vec3.cpp
PUBLIC

#common/app.h
#common/appplatformlistener.h
common/common_header.h
common/CommonTypes.h
common/Dungeons.h
common/entityuniqueid.h
common/SharedConstants.h
common/GameVersion.h

common/platform/file.h
#common/platform/activationuri.h
#common/platform/urilistener.h
#common/platform/threading/workerpool.h
#common/platform/appplatform.h
#common/platform/log.h
#common/platform/imagepickingcallback.h
#common/platform/threading/threadlocal.h
#common/platform/threading/spscqueue.h

common/locale/I18n.h

common/nbt/ByteArrayTag.h
common/nbt/ByteTag.h
common/nbt/CompoundTag.h
common/nbt/DoubleTag.h
common/nbt/EndTag.h
common/nbt/FloatTag.h
common/nbt/Int64Tag.h
common/nbt/IntArrayTag.h
common/nbt/IntTag.h
common/nbt/ListTag.h
common/nbt/NamedTag.h
common/nbt/NbtIo.h
common/nbt/ShortTag.h
common/nbt/StringTag.h
common/nbt/Tag.h
common/nbt/TagMemoryChunk.h

common/network/BinaryStream.h

common/util/ArrayWithLength.h
common/util/Base64.h
common/util/Bezier.h
common/util/BlockListSerializer.h
common/util/Bounds.h
common/util/Boxed.h
common/util/ClassID.h
common/util/CollectionUtils.h
common/util/ColorFormat.h
common/util/DataIO.h
common/util/DebugStats.h
common/util/Easing.h
common/util/EntityUtil.h
common/util/EnumHash.h
common/util/FilePickerInterface.h
common/util/FilePickerSettings.h
common/util/FloatRange.h
common/util/GridArea.h
common/util/GzipUtils.h
common/util/ItemListSerializer.h
common/util/JSONReadVerifier.h
common/util/JSONUtils.h
common/util/KeySpan.h
common/util/KeyValueInput.h
common/util/LootTableUtil.h
common/util/Math.h
common/util/MovePriorityQueue.h
common/util/MPMCQueue.h
common/util/NewType.h
common/util/NibblePair.h
common/util/ParameterStringify.h
common/util/PoolAllocator.h
common/util/RakDataIO.h
common/util/Random.h
common/util/range.h
common/util/RetryDelay.h
common/util/RunningAverage.h
common/util/SemVersion.h
common/util/SharedLock.h
common/util/SmallSet.h
common/util/SmoothFloat.h
common/util/SpinLock.h
common/util/StringByteInput.h
common/util/StringByteOutput.h
common/util/StringHash.h
common/util/StringUtils.h
common/util/Token.h
common/util/VariantParameterList.h
common/util/WeighedRandom.h
common/util/ZLibUtil.h
common/util/PerfTimer.h

common/world/Container.h
common/world/Difficulty.h
common/world/Direction.h
common/world/Facing.h
common/world/Pos.h
common/world/attribute/Attribute.h
common/world/entity/Entity.h
common/world/entity/EntityClassTree.h
common/world/entity/EntityDefinitionIdentifier.h
common/world/entity/EntityTypes.h
common/world/entity/Mob.h
common/world/entity/player/Abilities.h
common/world/entity/player/Player.h
common/world/item/Item.h
common/world/item/ItemCategory.h
common/world/item/ItemInstance.h
common/world/level/BlockPatternBuilder.h
common/world/level/BlockPos.h
common/world/level/BlockSource.h
common/world/level/BlockSourceListener.h
common/world/level/ChunkBlockPos.h
common/world/level/ChunkPos.h
common/world/level/FoliageColor.h
common/world/level/GameType.h
common/world/level/GeneratorType.h
common/world/level/Level.h
common/world/level/LevelConstants.h
common/world/level/LevelListener.h
common/world/level/LevelSettings.h
common/world/level/LightLayer.h
common/world/level/Rect2D.h
common/world/level/Seasons.h
common/world/level/SubChunkBlockPos.h
common/world/level/Tick.h
common/world/level/Weather.h
common/world/level/biome/BeachBiome.h
common/world/level/biome/Biome.h
common/world/level/biome/BiomeDecorator.h
common/world/level/biome/BiomeInclude.h
common/world/level/biome/BiomeSource.h
common/world/level/biome/DesertBiome.h
common/world/level/biome/ExtremeHillsBiome.h
common/world/level/biome/FixedBiomeSource.h
common/world/level/biome/FlatBiome.h
common/world/level/biome/ForestBiome.h
common/world/level/biome/HellBiome.h
common/world/level/biome/IceBiome.h
common/world/level/biome/JungleBiome.h
common/world/level/biome/MesaBiome.h
common/world/level/biome/MobSpawnerData.h
common/world/level/biome/MobSpawnUtils.h
common/world/level/biome/MushroomIslandBiome.h
common/world/level/biome/MutatedBiome.h
common/world/level/biome/MutatedBirchForest.h
common/world/level/biome/MutatedSavannaBiome.h
common/world/level/biome/OceanBiome.h
common/world/level/biome/PlainsBiome.h
common/world/level/biome/RiverBiome.h
common/world/level/biome/SavannaBiome.h
common/world/level/biome/StoneBeachBiome.h
common/world/level/biome/SwampBiome.h
common/world/level/biome/TaigaBiome.h
common/world/level/biome/TheEndBiome.h
common/world/level/biome/TheEndBiomeDecorator.h
common/world/level/block/ActivatorRailBlock.h
common/world/level/block/AirBlock.h
common/world/level/block/AnvilBlock.h
common/world/level/block/BasePressurePlateBlock.h
common/world/level/block/BaseRailBlock.h
common/world/level/block/BeaconBlock.h
common/world/level/block/BedBlock.h
common/world/level/block/BedrockBlock.h
common/world/level/block/BeetrootBlock.h
common/world/level/block/Block.h
common/world/level/block/BlockInclude.h
common/world/level/block/BlockRenderLayer.h
common/world/level/block/BlockState.h
common/world/level/block/BlockVolume.h
common/world/level/block/BookshelfBlock.h
common/world/level/block/BorderBlock.h
common/world/level/block/BrewingStandBlock.h
common/world/level/block/BushBlock.h
common/world/level/block/ButtonBlock.h
common/world/level/block/CactusBlock.h
common/world/level/block/CakeBlock.h
common/world/level/block/CameraBlock.h
common/world/level/block/CarrotBlock.h
common/world/level/block/CauldronBlock.h
common/world/level/block/ChalkboardBlock.h
common/world/level/block/ChestBlock.h
common/world/level/block/ChorusFlowerBlock.h
common/world/level/block/ChorusPlantBlock.h
common/world/level/block/ClayBlock.h
common/world/level/block/ClothBlock.h
common/world/level/block/CocoaBlock.h
common/world/level/block/ColoredBlock.h
common/world/level/block/ComparatorBlock.h
common/world/level/block/CropBlock.h
common/world/level/block/DaylightDetectorBlock.h
common/world/level/block/DeadBush.h
common/world/level/block/DetectorRailBlock.h
common/world/level/block/DiodeBlock.h
common/world/level/block/DirtBlock.h
common/world/level/block/DispenserBlock.h
common/world/level/block/DoorBlock.h
common/world/level/block/DoublePlantBlock.h
common/world/level/block/DragonEggBlock.h
common/world/level/block/DropperBlock.h
common/world/level/block/EnchantingTableBlock.h
common/world/level/block/EnderChestBlock.h
common/world/level/block/EndGatewayBlock.h
common/world/level/block/EndPortalBlock.h
common/world/level/block/EndPortalFrameBlock.h
common/world/level/block/EndRodBlock.h
common/world/level/block/EntityBlock.h
common/world/level/block/FarmBlock.h
common/world/level/block/FenceBlock.h
common/world/level/block/FenceGateBlock.h
common/world/level/block/FireBlock.h
common/world/level/block/FlowerBlock.h
common/world/level/block/FlowerPotBlock.h
common/world/level/block/FurnaceBlock.h
common/world/level/block/GlassBlock.h
common/world/level/block/GrassBlock.h
common/world/level/block/GrassPathBlock.h
common/world/level/block/GravelBlock.h
common/world/level/block/HalfTransparentBlock.h
common/world/level/block/HayBlockBlock.h
common/world/level/block/HeavyBlock.h
common/world/level/block/HopperBlock.h
common/world/level/block/HugeMushroomBlock.h
common/world/level/block/IceBlock.h
common/world/level/block/InvisibleBlock.h
common/world/level/block/ItemFrameBlock.h
common/world/level/block/LadderBlock.h
common/world/level/block/LeafBlock.h
common/world/level/block/LevelEvent.h
common/world/level/block/LeverBlock.h
common/world/level/block/LightGemBlock.h
common/world/level/block/LiquidBlock.h
common/world/level/block/LiquidBlockDynamic.h
common/world/level/block/LiquidBlockStatic.h
common/world/level/block/LogBlock.h
common/world/level/block/MelonBlock.h
common/world/level/block/MetalBlock.h
common/world/level/block/MobSpawnerBlock.h
common/world/level/block/MonsterEggBlock.h
common/world/level/block/MultiTextureBlock.h
common/world/level/block/MushroomBlock.h
common/world/level/block/MyceliumBlock.h
common/world/level/block/NetherReactor.h
common/world/level/block/NetherReactorPattern.h
common/world/level/block/NetherWartBlock.h
common/world/level/block/NewLeafBlock.h
common/world/level/block/NewLogBlock.h
common/world/level/block/NoteBlock.h
common/world/level/block/ObserverBlock.h
common/world/level/block/ObsidianBlock.h
common/world/level/block/OldLeafBlock.h
common/world/level/block/OldLogBlock.h
common/world/level/block/OreBlock.h
common/world/level/block/PistonArmBlock.h
common/world/level/block/PistonBlock.h
common/world/level/block/PodzolBlock.h
common/world/level/block/PortalBlock.h
common/world/level/block/PotatoBlock.h
common/world/level/block/PoweredRailBlock.h
common/world/level/block/PressurePlateBlock.h
common/world/level/block/PrismarineBlock.h
common/world/level/block/PumpkinBlock.h
common/world/level/block/QuartzBlockBlock.h
common/world/level/block/RailBlock.h
common/world/level/block/RedstoneBlock.h
common/world/level/block/RedstoneLampBlock.h
common/world/level/block/RedStoneOreBlock.h
common/world/level/block/RedstoneTorchBlock.h
common/world/level/block/RedStoneWireBlock.h
common/world/level/block/ReedBlock.h
common/world/level/block/RepeaterBlock.h
common/world/level/block/RotatedPillarBlock.h
common/world/level/block/SandBlock.h
common/world/level/block/SandStoneBlock.h
common/world/level/block/Sapling.h
common/world/level/block/SeaLanternBlock.h
common/world/level/block/SignBlock.h
common/world/level/block/SkullBlock.h
common/world/level/block/SlabBlock.h
common/world/level/block/SlimeBlock.h
common/world/level/block/SnowBlock.h
common/world/level/block/SoulSandBlock.h
common/world/level/block/SpongeBlock.h
common/world/level/block/StainedGlassBlock.h
common/world/level/block/StainedGlassPaneBlock.h
common/world/level/block/StairBlock.h
common/world/level/block/StemBlock.h
common/world/level/block/StoneBlock.h
common/world/level/block/StoneButtonBlock.h
common/world/level/block/StonecutterBlock.h
common/world/level/block/StoneSlabBlock.h
common/world/level/block/StructureBlock.h
common/world/level/block/StructureVoid.h
common/world/level/block/TallGrass.h
common/world/level/block/ThinFenceBlock.h
common/world/level/block/TntBlock.h
common/world/level/block/TopSnowBlock.h
common/world/level/block/TorchBlock.h
common/world/level/block/TransparentBlock.h
common/world/level/block/TrapDoorBlock.h
common/world/level/block/TripWireBlock.h
common/world/level/block/TripWireHookBlock.h
common/world/level/block/VineBlock.h
common/world/level/block/WallBlock.h
common/world/level/block/WaterlilyBlock.h
common/world/level/block/WebBlock.h
common/world/level/block/WeightedPressurePlateBlock.h
common/world/level/block/WoodBlock.h
common/world/level/block/WoodButtonBlock.h
common/world/level/block/WoodSlabBlock.h
common/world/level/block/WoolCarpetBlock.h
common/world/level/block/WorkbenchBlock.h
common/world/level/block/entity/BlockEntity.h
common/world/level/block/entity/BlockEntityRendererId.h
common/world/level/chunk/ChunkRefCount.h
common/world/level/chunk/ChunkSource.h
common/world/level/chunk/ChunkState.h
common/world/level/chunk/ChunkViewSource.h
common/world/level/chunk/DataLayer.h
common/world/level/chunk/EmptyChunkSource.h
common/world/level/chunk/LevelChunk.h
common/world/level/chunk/LevelChunkFormat.h
common/world/level/chunk/MainChunkSource.h
common/world/level/chunk/NetworkChunkSource.h
common/world/level/chunk/PlayerChunkSource.h
common/world/level/chunk/PostprocessingManager.h
common/world/level/chunk/SubLevelChunkFormat.h
common/world/level/chunk/WorldLimitChunkSource.h
common/world/level/dimension/Dimension.h
common/world/level/dimension/HellDimension.h
common/world/level/dimension/NormalDimension.h
common/world/level/dimension/end/TheEndDimension.h
common/world/level/levelgen/feature/BirchFeature.h
common/world/level/levelgen/feature/BlockBlobFeature.h
common/world/level/levelgen/feature/CactusFeature.h
common/world/level/levelgen/feature/ClayFeature.h
common/world/level/levelgen/feature/DeadBushFeature.h
common/world/level/levelgen/feature/DesertWellFeature.h
common/world/level/levelgen/feature/DoublePlantFeature.h
common/world/level/levelgen/feature/EndGatewayFeature.h
common/world/level/levelgen/feature/EndIslandFeature.h
common/world/level/levelgen/feature/EndPodiumFeature.h
common/world/level/levelgen/feature/FancyTreeFeature.h
common/world/level/levelgen/feature/Feature.h
common/world/level/levelgen/feature/FlowerFeature.h
common/world/level/levelgen/feature/GlowStoneFeature.h
common/world/level/levelgen/feature/GroundBushFeature.h
common/world/level/levelgen/feature/HellFireFeature.h
common/world/level/levelgen/feature/HellSpringFeature.h
common/world/level/levelgen/feature/HugeMushroomFeature.h
common/world/level/levelgen/feature/IcePatchFeature.h
common/world/level/levelgen/feature/IceSpikeFeature.h
common/world/level/levelgen/feature/JungleTreeFeature.h
common/world/level/levelgen/feature/LakeFeature.h
common/world/level/levelgen/feature/MegaJungleTreeFeature.h
common/world/level/levelgen/feature/MegaPineTreeFeature.h
common/world/level/levelgen/feature/MegaTreeFeature.h
common/world/level/levelgen/feature/MelonFeature.h
common/world/level/levelgen/feature/MonsterRoomFeature.h
common/world/level/levelgen/feature/OakFeature.h
common/world/level/levelgen/feature/OreFeature.h
common/world/level/levelgen/feature/PineFeature.h
common/world/level/levelgen/feature/PumpkinFeature.h
common/world/level/levelgen/feature/ReedsFeature.h
common/world/level/levelgen/feature/RoofTreeFeature.h
common/world/level/levelgen/feature/SandFeature.h
common/world/level/levelgen/feature/SavannaTreeFeature.h
common/world/level/levelgen/feature/SpikeFeature.h
common/world/level/levelgen/feature/SpringFeature.h
common/world/level/levelgen/feature/SpruceFeature.h
common/world/level/levelgen/feature/SwampTreeFeature.h
common/world/level/levelgen/feature/TallGrassFeature.h
common/world/level/levelgen/feature/TreeFeature.h
common/world/level/levelgen/feature/VinesFeature.h
common/world/level/levelgen/feature/WaterlilyFeature.h
common/world/level/levelgen/flat/FlatLevelSource.h
common/world/level/levelgen/structure/BoundingBox.h
common/world/level/levelgen/structure/EndCityFeature.h
common/world/level/levelgen/structure/EndCityPieces.h
common/world/level/levelgen/structure/MineshaftFeature.h
common/world/level/levelgen/structure/MineshaftPieces.h
common/world/level/levelgen/structure/NetherBridgeFeature.h
common/world/level/levelgen/structure/NetherBridgePieces.h
common/world/level/levelgen/structure/OceanMonumentFeature.h
common/world/level/levelgen/structure/OceanMonumentPieces.h
common/world/level/levelgen/structure/RandomScatteredLargeFeature.h
common/world/level/levelgen/structure/ScatteredFeaturePieces.h
common/world/level/levelgen/structure/StrongholdFeature.h
common/world/level/levelgen/structure/StrongholdPieces.h
common/world/level/levelgen/structure/StructureFeature.h
common/world/level/levelgen/structure/StructureFeatureType.h
common/world/level/levelgen/structure/StructureManager.h
common/world/level/levelgen/structure/StructurePiece.h
common/world/level/levelgen/structure/StructurePieceFactory.h
common/world/level/levelgen/structure/StructurePieceType.h
common/world/level/levelgen/structure/StructureStart.h
common/world/level/levelgen/structure/StructureTemplate.h
common/world/level/levelgen/structure/StructureTemplateHelpers.h
common/world/level/levelgen/structure/TemplateStructurePiece.h
common/world/level/levelgen/structure/VillageFeature.h
common/world/level/levelgen/structure/VillagePieces.h
common/world/level/levelgen/synth/ImprovedNoise.h
common/world/level/levelgen/synth/PerlinNoise.h
common/world/level/levelgen/synth/PerlinSimplexNoise.h
common/world/level/levelgen/synth/SimplexNoise.h
common/world/level/levelgen/v1/EndGatewayChunkSource.h
common/world/level/levelgen/v1/FarlandsFeature.h
common/world/level/levelgen/v1/HellRandomLevelSource.h
common/world/level/levelgen/v1/LargeCaveFeature.h
common/world/level/levelgen/v1/LargeFeature.h
common/world/level/levelgen/v1/LargeHellCaveFeature.h
common/world/level/levelgen/v1/RandomLevelSource.h
common/world/level/levelgen/v1/TheEndRandomLevelSource.h
common/world/level/material/Material.h
common/world/level/material/MaterialType.h
common/world/level/newbiome/AddDeepOceanLayer.h
common/world/level/newbiome/AddEdgeLayer.h
common/world/level/newbiome/AddIslandLayer.h
common/world/level/newbiome/AddMushroomIslandLayer.h
common/world/level/newbiome/AddSnowLayer.h
common/world/level/newbiome/BiomeEdgeLayer.h
common/world/level/newbiome/BiomeInitLayer.h
common/world/level/newbiome/DownfallLayer.h
common/world/level/newbiome/DownfallMixerLayer.h
common/world/level/newbiome/FuzzyZoomLayer.h
common/world/level/newbiome/IslandLayer.h
common/world/level/newbiome/Layer.h
common/world/level/newbiome/RareBiomeSpotLayer.h
common/world/level/newbiome/RegionHillsLayer.h
common/world/level/newbiome/RemoveTooMuchOceanLayer.h
common/world/level/newbiome/RiverInitLayer.h
common/world/level/newbiome/RiverLayer.h
common/world/level/newbiome/RiverMixerLayer.h
common/world/level/newbiome/ShoreLayer.h
common/world/level/newbiome/SmoothLayer.h
common/world/level/newbiome/SmoothZoomLayer.h
common/world/level/newbiome/VoronoiZoom.h
common/world/level/newbiome/ZoomLayer.h
common/world/level/pathfinder/BinaryHeap.h
common/world/level/pathfinder/Node.h
common/world/level/pathfinder/Path.h
common/world/level/pathfinder/PathFinder.h
common/world/level/saveddata/SavedData.h
common/world/level/storage/DBChunkStorage.h
common/world/level/storage/DBStorage.h
common/world/level/storage/ExternalFileLevelStorage.h
common/world/level/storage/ExternalFileLevelStorageSource.h
common/world/level/storage/GameRules.h
common/world/level/storage/LevelData.h
common/world/level/storage/LevelStorage.h
common/world/level/storage/LevelStorageSource.h
common/world/level/storage/LevelStorageState.h
common/world/level/storage/LevelSummary.h
common/world/level/storage/RegionFile.h
common/world/level/storage/SavedDataStorage.h
common/world/level/storage/StorageVersion.h
common/world/phys/AABB.h
common/world/phys/HitResult.h
common/world/phys/Vec2.h
common/world/phys/Vec3.h
common/world/phys/Vec4.h
common/world/redstone/Redstone.h
)

if(TARGET ZLIB::ZLIB)
    message(STATUS "Using existing ZLIB target")
else()
include(FetchContent)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY "https://github.com/madler/zlib.git"
    GIT_TAG "v1.3.2"
)

set(ZLIB_BUILD_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(zlib)

add_library(ZLIB::ZLIB ALIAS zlibstatic)

get_target_property(real_target ZLIB::ZLIB ALIASED_TARGET)
message(STATUS "ZLIB::ZLIB -> ${real_target}")


set(ZLIB_LIBRARY zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_ROOT ${zlib_SOURCE_DIR} CACHE PATH "" FORCE)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} CACHE PATH "" FORCE)
endif()


if(USEGUI)
target_compile_definitions(minecraftpe
    PRIVATE TAC_RENDER_SERVER_FIX=1
)
target_sources(minecraftpe 
PRIVATE
common/client/game/ExternalServersFile.cpp
common/client/renderer/block/AmbientOcclusionCalculator.cpp
common/client/renderer/block/BlockGraphics.cpp
common/client/renderer/block/BlockOccluder.cpp
common/client/renderer/block/BlockTessellator.cpp
common/client/renderer/block/BlockTessellatorCache.cpp
common/client/renderer/block/BlockTextureTessellator.cpp
common/client/renderer/chunks/BlockQueue.cpp
common/client/renderer/chunks/RangeIndices.cpp
common/client/renderer/chunks/RenderChunk.cpp
common/client/renderer/chunks/RenderChunkBuilder.cpp
common/client/renderer/chunks/RenderChunkSorter.cpp
common/client/renderer/chunks/VisibilityExtimator.cpp
common/client/renderer/chunks/VisibilityNode.cpp
common/client/renderer/renderer/Tessellator.cpp
common/client/renderer/texture/TextureAtlas.cpp
common/client/renderer/texture/TextureAtlasItem.cpp
common/client/renderer/texture/TextureAtlasTile.cpp
common/client/renderer/texture/TextureData.cpp
common/client/renderer/texture/TextureTessellator.cpp
common/client/renderer/texture/TextureUVCoordinateSet.cpp
PUBLIC
common/client/game/ExternalServersFile.h
common/client/game/NullVibration.h
common/client/game/Vibration.h
common/client/renderer/block/AmbientOcclusionCalculator.h
common/client/renderer/block/BlockGraphics.h
common/client/renderer/block/BlockOccluder.h
common/client/renderer/block/BlockTessellator.h
common/client/renderer/block/BlockTessellatorCache.h
common/client/renderer/block/BlockTextureTessellator.h
common/client/renderer/chunks/BlockQueue.h
common/client/renderer/chunks/RangeIndices.h
common/client/renderer/chunks/RenderChunk.h
common/client/renderer/chunks/RenderChunkBuilder.h
common/client/renderer/chunks/RenderChunkSorter.h
common/client/renderer/chunks/VisibilityExtimator.h
common/client/renderer/chunks/VisibilityNode.h
common/client/renderer/renderer/MinimumCoveringRectangles.h
common/client/renderer/renderer/PrimitiveMode.h
common/client/renderer/renderer/TerrainLayer.h
common/client/renderer/renderer/Tessellator.h
common/client/renderer/texture/TextureAtlas.h
common/client/renderer/texture/TextureAtlasItem.h
common/client/renderer/texture/TextureAtlasTile.h
common/client/renderer/texture/TextureData.h
common/client/renderer/texture/TextureTessellator.h
common/client/renderer/texture/TextureUVCoordinateSet.h
)


target_link_libraries(minecraftpe PUBLIC
ZLIB::ZLIB)

endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/gsl")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/raknet" "${CMAKE_CURRENT_BINARY_DIR}/raknet")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/Core" "${CMAKE_CURRENT_BINARY_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/leveldb" "${CMAKE_CURRENT_BINARY_DIR}/leveldb")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/json" "${CMAKE_CURRENT_BINARY_DIR}/json")



if(USEGUI)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/glad" "${CMAKE_CURRENT_BINARY_DIR}/glad")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/glfw" "${CMAKE_CURRENT_BINARY_DIR}/glfw")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../src-deps/Renderer" "${CMAKE_CURRENT_BINARY_DIR}/Renderer")
target_link_libraries(minecraftpe PUBLIC Renderer)
endif()

target_link_libraries(minecraftpe PUBLIC Core raknet leveldb json glfw glad)