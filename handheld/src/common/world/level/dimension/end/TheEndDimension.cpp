#include "common_header.h"

#include "world/level/dimension/end/TheEndDimension.h"
#include "world/level/LevelConstants.h"
#include "world/level/Level.h"
#include "world/level/biome/BiomeSource.h"
#include "platform/threading/ThreadLocal.h"
#include <world/level/biome/FixedBiomeSource.h>

TheEndDimension::TheEndDimension(Level& level) :
	Dimension(level, DimensionId::TheEnd, 256) {

	//setHasWeather(true);
// 	mSkyLightLayer.setSurrounding(Brightness::MAX);

	mSeaLevel = 0;
 	mBiomeSource = ThreadLocal<BiomeSource>([&level] {
 		return make_unique<FixedBiomeSource>(level.getSeed(),Biome::sky->mId);
 	});
}

TheEndDimension::~TheEndDimension() {

}

void TheEndDimension::init() {
	Dimension::init();
}

std::string TheEndDimension::getName() const {
	return "TheEnd";
}

bool TheEndDimension::isNaturalDimension() const {
	return false;
}

bool TheEndDimension::isValidSpawn(int x, int z) const {
	return false;
}

bool TheEndDimension::showSky() const {
	return false;
}

float TheEndDimension::getTimeOfDay(int time, float a) const {
	return 0.5f;
}

Color TheEndDimension::getFogColor(float br) const {
	return Color(0.2f, 0.03f, 0.03f);
}

float TheEndDimension::getFogDistanceScale() const {
	return 0.4f;
}

bool TheEndDimension::isFoggyAt(int x, int z) const {
	return false;
}

bool TheEndDimension::mayRespawn() const {
	return false;
}

Vec3 TheEndDimension::translatePosAcrossDimension(const Vec3& originalPos, DimensionId fromId) const {
	const float scale = 8.0f;
	Vec3 newPos(originalPos);

	if (fromId == DimensionId::Nether) {
		newPos.x *= scale;
		newPos.z *= scale;
	}
 	if (fromId == DimensionId::Overworld) {
		newPos = mLevel.getSharedSpawnPos();
 	}

	newPos.x = (float)Math::clamp((int)newPos.x, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);
	newPos.z = (float)Math::clamp((int)newPos.z, -Level::MAX_LEVEL_SIZE + 128, Level::MAX_LEVEL_SIZE - 128);

	return newPos;
}
