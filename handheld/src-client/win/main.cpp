#include "main_win32.h"
#include <Debug/Log.h>
#include <platform/file.h>
#include <world/level/Level.h>
#include "world/level/dimension/Dimension.h"
#include "world/level/chunk/ChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/storage/LevelStorage.h"
#include <world/level/chunk/MainChunkSource.h>
#include "world/level/biome/Biome.h"
#include "world/level/material/Material.h"
#include "util/PerfTimer.h"
#include "client/renderer/texture/TextureAtlas.h"

#include <fstream>
#include <cstdint>
#include <vector>
#include <renderer/block/BlockTessellator.h>
#include <world/level/storage/DBStorage.h>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

//not needed since dbstorage is working now
/*
class StubLevelStorage : public LevelStorage 
{
public:

	virtual ~StubLevelStorage()
	{

	}

	virtual bool loadLevelData(LevelData& data)
	{
		return false;
	}

	virtual Unique<ChunkSource> createChunkStorage(Unique<ChunkSource> generator, StorageVersion v = SharedConstants::CurrentStorageVersion)
	{
		return make_unique<MainChunkSource>(nullptr);
	}

	virtual void saveLevelData(LevelData& levelData)
	{

	}

	virtual const std::string& getFullPath() const
	{
		return "um some path here";
	}

	virtual void savePlayerData(const std::string& key, std::string&& data)
	{

	}

	virtual void saveData(const std::string& key, std::string&& data)
	{

	}

	// Clones tag data to another key.  Returns true on success
	//virtual bool clonePlayerData(const std::string& fromKey, const std::string& toKey)
	//{

	//}

	virtual bool isCorrupted() const
	{
		return false;
	}

	//virtual std::string loadData(const std::string& key)
	//{

	//}

	virtual LevelStorageResult getState() const
	{
		return LevelStorageResult{ LevelStorageState::Unknown,"" };
	}

	static const std::string LOCAL_PLAYER_TAG;
	virtual Unique<CompoundTag> loadPlayerData(const std::string& clientUniqueName, const std::string& oldIdentifier = Util::EMPTY_STRING)
	{
		return make_unique<CompoundTag>(nullptr);
	}
	virtual Unique<CompoundTag> loadLocalPlayerData()
	{
		return make_unique<CompoundTag>(nullptr);
	}

	virtual std::vector<std::string> loadAllPlayerIDs()
	{
		std::vector<std::string> b;
		b.push_back("player");

		return b;
	}

	virtual void save(Player& player)
	{

	}
	virtual void save(Entity& entity)
	{

	}

	virtual std::string getLevelId() const
	{
		return "some id here";
	}

	virtual std::vector<std::string> pauseModificationsAndGetFiles()
	{
		std::vector<std::string> b;

		return b;
	}
	virtual void resumeModifications()
	{

	}

	virtual void compactStorage()
	{

	}
	virtual void suspendStorage()
	{

	}
	virtual void resumeStorage()
	{

	}

	//Unique<CompoundTag> loadServerPlayerData(const Player& client);

	//void saveData(const std::string& key, const CompoundTag& tag);
	//void savePlayerData(const std::string& key, const CompoundTag& tag);

private:
	virtual void _savePlayerDataSync(const std::string& key, std::string&& data)
	{

	}

};*/



#pragma pack(push, 1)
struct BMPHeader
{
	uint16_t type = 0x4D42; // "BM"
	uint32_t size;
	uint16_t reserved1 = 0;
	uint16_t reserved2 = 0;
	uint32_t offset = 54;

	uint32_t dibSize = 40;
	int32_t width;
	int32_t height;
	uint16_t planes = 1;
	uint16_t bpp = 24;
	uint32_t compression = 0;
	uint32_t imageSize;
	int32_t xppm = 2835;
	int32_t yppm = 2835;
	uint32_t colorsUsed = 0;
	uint32_t colorsImportant = 0;
};
#pragma pack(pop)

struct RGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};





void writeBMP(const char* filename, int width, int height, const RGB* pixels)
{
	std::ofstream file(filename, std::ios::binary);
	int rowSize = (width * 3 + 3) & ~3;
	int imageSize = rowSize * height;
	BMPHeader header;
	header.width = width;
	header.height = height;
	header.imageSize = imageSize;
	header.size = 54 + imageSize;

	file.write(reinterpret_cast<char*>(&header), sizeof(header));

	std::vector<uint8_t> row(rowSize);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			RGB pixel = pixels[y * width + x];
			row[x * 3 + 0] = pixel.b;
			row[x * 3 + 1] = pixel.g;
			row[x * 3 + 2] = pixel.r;
		}

		file.write(reinterpret_cast<char*>(row.data()), rowSize);
	}
}


RGB getBlockColour(BlockID id)
{
	switch (id)
	{
	case 0: return { 255,255,255 };
	case 1: return { 128,128,128 };
	case 3: return { 130,90,50 };
	case 2: return { 90,180,60 };
	case 7: return { 40,40,40 };
	case 8:
	case 9: return { 40,80,200 };
	case 10:
	case 11: return { 255,60,0 };
	case 87: return { 180, 10, 0 };
	default:
		Color c = Block::mBlocks[id.value]->getMapColor();//good thing minecraft has this built in
		RGB d;
		d.r = c.r * 255;
		d.g = c.g * 255;
		d.b = c.b * 255;
		return d;
	}

	/*switch (id)
	{
	case 0: return { 255,255,255 };
	case 1: return { 128,128,128 };
	case 3: return { 130,90,50 };
	case 2: return { 90,180,60 };
	case 7: return {40,40,40};
	case 8:
	case 9: return { 40,80,200 };
	case 10:
	case 11: return { 255,60,0 };
	case 87: return { 180, 10, 0};
	default: return { 255,0,255 }; // unknown = pink
	}*/
}

class Token
{
public:
	bool isOption = false;
	std::string str;
};

bool Tokenise(const std::string& cmd, std::vector<Token>& tokens)
{
	Token t;
	char inQuotes = '\0';
	bool prevSlash = false;
	for (char c : cmd)
	{

		bool oldPrevSlash = prevSlash;
		prevSlash = !prevSlash && (c == '\\');
		if (inQuotes == '\0')
		{
			if (isspace(c))
			{
				if (!t.str.empty())
				{
					tokens.push_back(t);
					t.str.erase();
					t.isOption = false;
				}
				continue;
			}
			if ((c == '\"' || c == '\''))
			{
				if (!oldPrevSlash)
				{
					inQuotes = c;
					continue;
				}
				else
				{
					t.str.pop_back();//remove
				}
			}

		}
		else if (((c == '\"' && inQuotes == '\"') || (c == '\'' && inQuotes == '\'')))
		{
			if (!oldPrevSlash)
			{
				inQuotes = '\0';
				continue;
			}
			else
			{
				t.str.pop_back();//remove
			}
		}

		/*if (c == '-' && prevChar == '-' && !inQuotes)
		{
			t.str.pop_back();
			t.isOption = true;
		}
		prevChar = c;*/

		t.str.push_back(c);

	}
	if (!t.str.empty())
	{
		tokens.push_back(t);
	}
	return inQuotes == '\0';//quotes should be closed
}
std::string strlower(std::string str)
{
	std::string news = str;
	std::transform(news.begin(), news.end(), news.begin(), [](char c) {return tolower(c); });
	return news;
}


struct CCTX
{
	bool running;
	Level* level;
	Dimension* dim;
};

CCTX commandContext;

typedef bool(*TestMcCmd)(CCTX * commandctx, std::vector<std::string> &options, std::vector<std::string>& params );//options get the -- removed

struct McCmdStruct
{
	std::string cmdName;
	TestMcCmd cmd;
	int minParams;

};

bool mcmd_dimension(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	DimensionId did = Undefined;
	std::string l = strlower(params[0]);
	if (l == "overworld")
		did = Overworld;
	else if (l == "nether")
		did = Nether;
	else if (l == "end" || l == "theend")
		did = TheEnd;
	commandctx->dim = &commandctx->level->createDimension(did);
	return true;
}

bool mcmd_help(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	printf(R"(Help
Commands:
dimension [overworld/nether/end] - change dimension
loadchunks {--profile} x z {x2 z2} - load chunks from x z to x2 z2 or x z to x+1 z+1, if profile is specified it will generate a report on the loading times
writechunk {--topdown} {--joinimages} {--generatechunk} x z {x2 z2} - write chunk at x z to bmp, if topdown is specified it will make a top down view, joinimages makes a big view, generate chunk generates instead of only loading
save - save+flush leveldb to filesystem
exit - exit this program
chunkinfo - get info on chunks
blockinfo - get info on a block
seed - get seed of level
profile [start/report] - control the profiler (mainly for level loading/generation etc)
render - start the renderer
)");
	return true;
}

bool mcmd_loadchunks(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	ChunkPos cp(atoi(params[0].c_str()), atoi(params[1].c_str()));
	ChunkPos cp2(cp.x + 1, cp.z + 1);

	//bool forceGenerate = false;
	bool profile = false;
	for (int i = 0; i < options.size(); i++)
	{
		//forceGenerate |= (strlower(options[i]) == "forcegenerate");
		profile |= (strlower(options[i]) == "profile");
	}

	if (params.size() >= 4)
	{
		cp2 = ChunkPos(atoi(params[2].c_str()), atoi(params[3].c_str()));
	}
	if(profile)
	ScopedProfilee::beginreport();
	for (int i = cp.x; i < cp2.x; i++)
	{
		for (int j = cp.z; j < cp2.z; j++)
		{
			/*if (forceGenerate)
			{
				LevelChunk* chunk = commandctx->dim->getChunkSource().getOrLoadChunk(ChunkPos(i, j), ChunkSource::LoadMode::None);
				if (chunk)
				{
					while (true)
					{
						if (commandctx->dim->getChunkSource().releaseChunk(*chunk))
							break;
					}
				}
			}*/
			commandctx->dim->getChunkSource().getOrLoadChunk(ChunkPos(i, j), ChunkSource::LoadMode::Deferred);

		}
	}

	if (profile)
	{
		std::cout << "Profile report:\n";
		ScopedProfilee::report();
	}
	return true;
}

bool mcmd_writechunk(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	char buff[1000];

	bool topDown = false;
	bool joinImages = false;
	bool loadChunks = false;
	for(int i = 0; i < options.size();i++)
	{
		topDown |= (strlower(options[i]) == "topdown");//top down instead of side view
		joinImages |= (strlower(options[i]) == "joinimages");//instead of making images of multiple chunks seperately, combine them. this+topdown makes a map-like view
		loadChunks |= (strlower(options[i]) == "generatechunk");//generate the chunk instead of only loading one
	}
	ChunkPos cp(atoi(params[0].c_str()), atoi(params[1].c_str()));
	ChunkPos cp2(cp.x + 1, cp.z + 1);
	if (params.size() >= 4)
	{
		cp2 = ChunkPos(atoi(params[2].c_str()), atoi(params[3].c_str()));
	}

	std::vector<RGB> combined;

	int chXSpan = (cp2.x - cp.x);
	int chZSpan = (cp2.z - cp.z);

	if (joinImages)
	{
		combined.resize(topDown ? 16 * 16 * chXSpan * chZSpan : 256 * 16 * 16 * chXSpan * chZSpan);
	}
	else
	{
		combined.resize(topDown ? 16*16 : 256*16);
	}
	int fullWidth = (cp2.x - cp.x) * 16;
	for (int i = cp.x; i < cp2.x; i++)
	{
		for (int j = cp.z; j < cp2.z; j++)
		{
			ChunkPos cpl(i, j);
			LevelChunk* chunk = commandctx->dim->getChunkSource().getOrLoadChunk(cpl, loadChunks ? ChunkSource::LoadMode::Deferred : ChunkSource::LoadMode::None);
			if (chunk)
			{
				if (!joinImages)
				{
					sprintf(buff, "./restore/output/%s/chk%i-%i/", commandctx->dim->getName().c_str(), cpl.x, cpl.z);
					std::filesystem::create_directories(
						std::filesystem::path(buff)
					);
				}
					//makePicturesOfChunk(chunk, commandctx->dim,combined, topDown,joinImages,i-cp.x,j-cp.z, (cp2.x - cp.x) *16);

				int chX = i - cp.x;
				int chZ = j - cp.z;


				if (topDown)
				{
					for (uint8_t x = 0; x < 16; x++)
					{
						for (uint8_t z = 0; z < 16; z++)
						{
							Height h = chunk->getHeightmap({ x,255,z });//good thing there is a heightmap

							Height mn = (Height)(((int)h != 0) ? h - 1 : 0);

							BlockID bid = chunk->getBlock({ x,mn,z });
							/*for (Height y = 255; y >= 0; y--)
							{
								bid = chunk->getBlock({ x,y,z });
								if (bid.value != 0)
								{
									break;
								}

							}*/

							if (joinImages)
								combined[(((chZ * 16) + z) * fullWidth) + (chX * 16) + x] = getBlockColour(bid);
							else
								combined[(z * 16) + x] = getBlockColour(bid);
						}
					}

					if (!joinImages)
					{
						sprintf(buff, "./restore/output/%s/chk%i-%i/TopLayer.bmp", commandctx->dim->getName().c_str(), cpl.x, cpl.z);


						writeBMP(buff, 16, 16, combined.data());
						printf("Writen to %s\n", buff);
					}
				}
				else
				{
					for (uint8_t z = 0; z < 16; z++)
					{

						for (Height y = 0; y < 256; y++)
						{

							for (uint8_t x = 0; x < 16; x++)
							{
								if (joinImages)
									combined[((chZ * 16 * 256 + z * 256 + y) * fullWidth) + (chX * 16 + x)] = getBlockColour(chunk->getBlock({ x,y,z }));//I don't like ((chZ * 16 * 256 + z * 256 + y) * fullWidth) + (chX * 16 + x)
								else
									combined[(y * 16) + x] = getBlockColour(chunk->getBlock({ x,y,z }));
							}
						}


						if (!joinImages)
						{
							sprintf(buff, "./restore/output/%s/chk%i-%i/SideLayer%i.bmp", commandctx->dim->getName().c_str(), cpl.x,cpl.z, z);

							writeBMP(buff, 16, 256, combined.data());
							printf("Writen to %s\n", buff);
						}
					}
				}
			}
			else
			{
				std::cout << "Error: this chunk is unloaded. use --generatechunk or the loadchunks command\n";
			}

		}
	}

	if (joinImages)
	{
			sprintf(buff, "./restore/output/%s/chk%i-%i-%i-%i/", commandctx->dim->getName().c_str(), cp.x, cp.z,cp2.x,cp2.z);
			std::filesystem::create_directories(
				std::filesystem::path(buff)
			);
		if (topDown)
		{
			sprintf(buff, "./restore/output/%s/chk%i-%i-%i-%i/TopLayer.bmp", commandctx->dim->getName().c_str(), cp.x, cp.z, cp2.x, cp2.z);

			writeBMP(buff, 16*chXSpan, 16*chZSpan, combined.data());
			printf("Writen to %s\n", buff);
		}
		else
		{
			for (int i = 0; i < chZSpan;i++)
			{
				sprintf(buff, "./restore/output/%s/chk%i-%i-%i-%i/SideLayer%i.bmp", commandctx->dim->getName().c_str(), cp.x, cp.z, cp2.x, cp2.z,i);

				writeBMP(buff, 16 * chXSpan, 256, combined.data() + (i * 256 * fullWidth));
				printf("Writen to %s\n", buff);
			}
		}
	}
	return true;
}

bool mcmd_save(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	commandctx->level->saveDirtyChunks();

	//mStorage->mDb->CompactRange(nullptr, nullptr);
	//dim.getChunkSource().saveLiveChunk(*chunks[0]);
	for (int id = DimensionId::Overworld; id < DimensionId::Count; id++)
	{
		Dimension* dim = commandctx->level->getDimension((DimensionId)id);
		if (dim)
			dim->getChunkSource().compact();//compacts database
	}
	return true;
}

bool mcmd_exit(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	commandctx->running = false;
	return true;
}

bool mcmd_profile(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	if (strlower(params[0]) == "start")
	{
		ScopedProfilee::beginreport();
		return true;
	}
	else if (strlower(params[0]) == "report")
	{
		ScopedProfilee::report();
		return true;
	}
	else
	{
		std::cout << "Invalid option";
		return false;
	}
}


bool mcmd_chunkinfo(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	ChunkPos cp(atoi(params[0].c_str()), atoi(params[1].c_str()));
	LevelChunk* chunk = commandctx->dim->getChunkSource().getOrLoadChunk(cp, ChunkSource::LoadMode::None);
	if (chunk)
	{
		const char* states[] = {
				"Unloaded",
	"Generating",
	"Generated",
	"PostProcessing",
	"Loaded"
		};


		std::cout << "Chunk State: " << states[(int)chunk->getState().load()] << "\n";

		std::vector<std::pair<int,int>> biomes(Biome::BIOMES_COUNT,std::pair<int,int>(-1,0));

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				Biome& biome = chunk->getBiome(ChunkBlockPos(x, 0, z));
				biomes[biome.mId].first = biome.mId;
				biomes[biome.mId].second++;
			}
		}

		std::sort(biomes.begin(), biomes.end(), [](std::pair<int, int>& a, std::pair<int, int>& b) {
			return a.second > b.second;
			});
		std::cout << "Biome composition:\n";
		for (int i = 0; i < biomes.size();i++)
		{
			if (biomes[i].first == -1)
				break;

			float percentage = ((float)biomes[i].second / 256)*100;

			std::cout << "Biome " << Biome::getBiome(biomes[i].first)->mName << " makes up " << percentage << "% of chunk\n";
		}



	}
	else
	{
		std::cout << "Chunk unloaded\n";
	}
	return true;
}

bool mcmd_blockinfo(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	return true;
}

bool mcmd_seed(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	std::cout << "Seed: " << commandctx->level->getSeed() << "\n";
	return true;
}
void renderThreadStuff();


bool mcmd_render(CCTX* commandctx, std::vector<std::string>& options, std::vector<std::string>& params)
{
	std::thread renderThread(renderThreadStuff);
	renderThread.detach();
	return true;
}




McCmdStruct cmds[]= {
	{"dimension",mcmd_dimension,1},
	{"help",mcmd_help,0},
	{"loadchunks",mcmd_loadchunks,2},
	{"writechunk",mcmd_writechunk,2},
	{"save",mcmd_save,0},
	{"exit", mcmd_exit, 0},
	{"chunkinfo", mcmd_chunkinfo, 2},
	{"blockinfo", mcmd_blockinfo, 3},
	{"seed", mcmd_seed, 0},
	{"profile", mcmd_profile,1},
	{"render",mcmd_render,0},
};



void cmdThread(Level * level)
{
	printf("MCPE CONSOLE\n");
	printf("Use the 'help' command to get started.\n");



	commandContext.level = level;
	commandContext.dim = &level->createDimension(Overworld);
	commandContext.running = true;

	while (commandContext.running)
	{
		std::string command;
		std::cout << "~$ ";
		std::getline(std::cin, command);
		std::vector<Token> tokenised;
		bool succeded = Tokenise(command, tokenised);
		if (succeded)
		{

			if (tokenised.size() >= 1)
			{
				std::vector<std::string> parameters;
				std::vector<std::string> options;
				for (int i = 1; i < tokenised.size(); i++)
				{
					if (tokenised[i].str.size() > 2 && tokenised[i].str[0] == '-' && tokenised[i].str[1] == '-')
					{
						options.push_back(tokenised[i].str.substr(2));
					}
					else
					{
						parameters.push_back(tokenised[i].str);
					}
				}
				bool found = false;
				for (int i = 0; i < ARRAYSIZE(cmds); i++)
				{
					if (cmds[i].cmdName == strlower(tokenised[0].str))
					{
						found = true;
						if (cmds[i].minParams > parameters.size())
						{
							std::cout << "Missing parameters, at least " << cmds[i].minParams << " needed.\n";
							break;
						}
						if (cmds[i].cmd(&commandContext, options, parameters))
						{

						}
						break;
					}
				}
				if (!found)
					std::cout << "Invalid command " << tokenised[0].str << "\n";
			}
		}
		else
		{
			std::cout << "Failed to tokenise\n";
		}
		//for (auto& token : tokenised)
		//{
		//	std::cout << "Token " << token.str << "\n";
		//}

	}
}

void renderThreadStuff();
//void mcpe_re_init()











TextureData mcperes_loadTexture(const ResourceLocation& resourceLocation)
{
	if (resourceLocation.mFileSystem == ResourceFileSystem::UserPackage)
	{
		std::string fullpath = "./resource_packs/vanilla/" + resourceLocation.mPath;

		mce::ImageDescription desc;

		int channels;

		unsigned char* dat = stbi_load((fullpath+".png").c_str(), &desc.mWidth, &desc.mHeight, &channels, 0);//fix

		if(dat == nullptr) dat = stbi_load((fullpath + ".tga").c_str(), &desc.mWidth, &desc.mHeight, &channels, 0);//fix

		if (dat)
		{
			int datalen = channels * desc.mWidth * desc.mHeight;

			desc.mTextureFormat = channels == 4 ? mce::TextureFormat::R8G8B8A8_UNORM : mce::TextureFormat::R8G8B8_UNORM;
			desc.mMipMapCount = 1;

			TextureData data(desc);
			data.getDataPackedModify().resize(datalen);
			std::copy(dat, dat + datalen, data.getDataPackedModify().begin());
			stbi_image_free(dat);

			return std::move(data);
		}
	}
	return{ };
}










class UserPackageResourceLoader : public ResourceLoader
{
public:
	UserPackageResourceLoader() : ResourceLoader("./")
	{

	}

	virtual bool load(const ResourceLocation& resourceLocation, std::string& resourceStream)
	{
		if (resourceLocation.mFileSystem == ResourceFileSystem::UserPackage)
		{
			std::string fullpath = "./resource_packs/vanilla/" + resourceLocation.mPath;
			std::ifstream readfile(fullpath);
			if (readfile.is_open())
			{
				std::stringstream buffer;
				buffer << readfile.rdbuf();

				resourceStream = buffer.str();
				return true;
			}
		}
		return false;
	}
	virtual std::vector<std::string> loadAllVersionsOf(const ResourceLocation& resourceLocation)//dont support multiple resource packs yet
	{
		std::vector<std::string> retb{std::string()};
		if (load(resourceLocation, retb[0]))
			return retb;
		else
			return std::vector<std::string>();
	}

	TextureData loadTexture(const ResourceLocation& resourceLocation) 
	{
		return std::move(mcperes_loadTexture(resourceLocation));
	}
};

extern mce::TextureGroup texgroup;

void main()
{
	DebugLog::createLog("./restore/");
	LOGI("Minecraft restoration is running\n");

	Resource::registerLoader(ResourceFileSystem::UserPackage, make_unique<UserPackageResourceLoader>());

	Material::initMaterials();
	Block::initBlocks();
	Biome::initBiomes();




	Level* level = new Level(std::make_unique<DBStorage>("some_level","./restore/mcworld"));

	/*Dimension& dim = level->createDimension(Overworld);

	LevelChunk* chunks[9];
	//needed for post-processing
	static const ChunkPos offsets[] = {
	{ 0, 0 },
	{ -1, -1 },
	{ 0, -1 },
	{ 1, -1 },
	{ -1, 0 },
	{ 1, 0 },
	{ -1, 1 },
	{ 0, 1 },
	{ 1, 1 }
	};*/

	cmdThread(level);


    //while (true)
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
}

