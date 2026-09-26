//Thanks learnopengl.com (losely based of some of that code)

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <world/level/Level.h>
#include <renderer/chunks/RenderChunkBuilder.h>
#include "world/level/dimension/Dimension.h"
#include <renderer/game/RenderChunkRenderParameters.h>
#include "renderer/chunks/RenderChunk.h"
#include "renderer/renderer/Tessellator.h"
#include "world/level/Chunk/ChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "renderer/texture/TextureAtlas.h"
#include "shader.h"
#include <world/level/FoliageColor.h>
#include "renderer/renderer/RenderMaterialGroup.h"


int screenx = 1600;
int screeny = 900;

//Shader* sptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);//Set the height again
	screenx = width;
	screeny = height;

	glm::mat4 projection = glm::perspective(glm::radians(85.0f),
		(float)screenx / (float)screeny,
		0.1f, 1000.0f);

	//sptr->setMatrix4("projection", glm::value_ptr(projection));

}

class GLInput
{
public:
	virtual bool KeyDown(int key);
	virtual bool KeyUp(int key);
	virtual bool MouseDown(int key);
	virtual bool MouseUp(int key);
	virtual bool NewKeyDown(int key);
	virtual bool NewKeyUp(int key);
	virtual bool NewMouseDown(int key);
	virtual bool NewMouseUp(int key);
	void Clear();

	bool lookup_table[GLFW_KEY_LAST];
	int last_table[GLFW_KEY_LAST];
	bool mlookup_table[GLFW_MOUSE_BUTTON_LAST];
	int mlast_table[GLFW_MOUSE_BUTTON_LAST];
	float mouseXPos;
	float mouseYPos;
} glinput;

bool GLInput::KeyDown(int key)
{
	return lookup_table[key] == GLFW_PRESS;
}

bool GLInput::KeyUp(int key)
{
	return lookup_table[key] == GLFW_RELEASE;
}

bool GLInput::NewKeyDown(int key)
{
	return last_table[key] == 1;
}

bool GLInput::NewKeyUp(int key)
{
	return last_table[key] == -1;
}

bool GLInput::MouseDown(int key)
{
	return mlookup_table[key] == GLFW_PRESS;
}

bool GLInput::MouseUp(int key)
{
	return mlookup_table[key] == GLFW_RELEASE;
}

bool GLInput::NewMouseDown(int key)
{
	return mlast_table[key] == 1;
}

bool GLInput::NewMouseUp(int key)
{
	return mlast_table[key] == -1;
}

void GLInput::Clear()
{
	memset(mlast_table, 0, GLFW_MOUSE_BUTTON_LAST * sizeof(int));
	memset(last_table, 0, GLFW_KEY_LAST * sizeof(int));
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	bool pressed = (action != GLFW_RELEASE);

	if (pressed != glinput.lookup_table[key])
	{
		glinput.last_table[key] = pressed ? 1 : -1;
		glinput.lookup_table[key] = pressed;
	}
}


struct CCTX
{
	std::atomic<bool> running;
	Level* level;
	Dimension* dim;
};

extern CCTX commandContext;

mce::TextureGroup texgroup;

class Camera
{
public:
	glm::vec3 mPos = glm::vec3(0, 100, 0);
	glm::vec3 cameraFront = glm::vec3(0, -1, 0);
	glm::vec3 cameraMoveFront = glm::vec3(1, 0, 0);
	glm::vec3 cameraUp = glm::vec3(0, 1, 0);



	double lastX;
	double lastY;
	bool firstMouse = true;
	double yaw = 0;
	double pitch = 0;

	BlockSource* mRegion;//commandContext.dim->getChunkSource()
	Level* mLevel;
	Vec3 mPosDelta;
	float mMaxAutoStep = 0.0f;
	bool mCollidableMobNear = false;
	float mLastPenetration = 0;
	bool mHorizontalCollision = false;
	bool mVerticalCollision = false;
	bool mCollision = false;
	bool mStuckInCollider = false;
	bool mOnGround = true;
	bool mIsStuckInWeb = false;
	bool mNoGravity = false;
	//AABB* mLastHitBB;
	AABB mBB;
	std::vector<AABB> mSubBBs;
	//Vec2 mBBDim = { 0.6f, 1.8f }; // *** Def ***
	float mHeightOffset = 1.7;
	Vec3 mSlideOffset = Vec3(0,0,0);
	bool mNoPhysics = false;

	BlockSource& getRegion() { return *mRegion; };
	Level& getLevel() { return *mLevel; };
	bool isStackable() { return false; };
	void moveBBs(const Vec3& pos);

	void move(const Vec3& posIn);

	Camera(Level* level, Dimension* dim) : mPosDelta(0, 0, 0)
	{
		mRegion = &dim->getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD();
		mLevel = level;
		mBB.set(-0.4,98,-0.4,0.4,99.8,0.4);
	}

	bool isSneaking()
	{
		return glinput.KeyDown(GLFW_KEY_LEFT_SHIFT) || glinput.KeyDown(GLFW_KEY_RIGHT_SHIFT);
	}
	//Note this is quite hacked together - it REALLY needs to be updated at a constant tickrate - thats next
	void UpdateCam(float deltaTime)
	{
		float xposIn = glinput.mouseXPos;
		float yposIn = glinput.mouseYPos;


		if (firstMouse)
		{
			lastX = xposIn;
			lastY = yposIn;
			firstMouse = false;
		}

		float xoffset = xposIn - lastX;
		float yoffset = lastY - yposIn;
		lastX = xposIn;
		lastY = yposIn;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

		direction.x = cos(glm::radians(yaw));
		direction.y = 0;
		direction.z = sin(glm::radians(yaw));


		cameraMoveFront = glm::normalize(direction);


		float cameraSpeed = 4.0f; //* deltaTime;


		

		if (glinput.KeyDown(GLFW_KEY_LEFT_CONTROL) || glinput.KeyDown(GLFW_KEY_RIGHT_CONTROL))
			cameraSpeed *= 3;

		if (!mOnGround)
		{
			cameraSpeed = 2;
		}

		const float MAGIC_FRICTION_CONSTANT = 0.91f;
		const float MAGIC_FRICTION_MODIFIER = 0.6f;

		float friction = MAGIC_FRICTION_CONSTANT;

		if (mOnGround) {
			friction = MAGIC_FRICTION_MODIFIER * MAGIC_FRICTION_CONSTANT;

			BlockID t = getRegion().getBlockID(
				Math::floor(mPos.x),
				Math::floor(mBB.min.y - 0.1f),
				Math::floor(mPos.z)
			);

			if (t > 0) {
				friction = Block::mBlocks[t]->getFriction() * MAGIC_FRICTION_CONSTANT;
			}
		}
		friction = std::pow(friction, deltaTime / 0.05f);




		

		if (mOnGround && glinput.NewKeyDown(GLFW_KEY_SPACE))
		{
			mPosDelta.y = 0.9;
		}

		glm::vec3 mmPos(0, 0, 0);//bps
		if (glinput.KeyDown(GLFW_KEY_W))
			mmPos += cameraSpeed * cameraMoveFront;
		if (glinput.KeyDown(GLFW_KEY_S) == GLFW_PRESS)
			mmPos -= cameraSpeed * cameraMoveFront;
		if (glinput.KeyDown(GLFW_KEY_A) == GLFW_PRESS)
			mmPos -= glm::normalize(glm::cross(cameraMoveFront, cameraUp)) * cameraSpeed;
		if (glinput.KeyDown(GLFW_KEY_D) == GLFW_PRESS)
			mmPos += glm::normalize(glm::cross(cameraMoveFront, cameraUp)) * cameraSpeed;
		mPosDelta += Vec3(mmPos.x,mmPos.y,mmPos.z) * deltaTime;


		if (!mNoGravity) {
			float tickScale = deltaTime / 0.05f;

			mPosDelta.y -= 0.08f * tickScale;
			mPosDelta.y *= std::pow(0.98f, tickScale);
		}

		mPosDelta.x *= friction;
		mPosDelta.z *= friction;

		//mPos += mmPos;
		move(mPosDelta*deltaTime*20);
	}
};




void Camera::moveBBs(const Vec3& pos) {
	mBB.move(pos);
	for (auto& bb : mSubBBs) {
		bb.move(pos);
	}
}

void Camera::move(const Vec3& posIn) {
	Vec3 speed(posIn);
	// stupid check
	if (std::abs(speed.x) > 500 || std::abs(speed.y) > 500 || std::abs(speed.z) > 500) {
		ALOGE(LOG_AREA_ENTITY, "Entity moved WAAAY too fast!\n");
		speed.x = speed.y = speed.z = 0;
	}

	if (mNoPhysics) {
		moveBBs(speed);
		mPos.x = (mBB.min.x + mBB.max.x) / 2.0f;
		mPos.y = mBB.min.y + mHeightOffset - mSlideOffset.y;
		mPos.z = (mBB.min.z + mBB.max.z) / 2.0f;
		return;
	}

	//ScopedProfile("move");

	// add all intersecting aabb's to one box
	std::vector<AABB>& preIntersectingAabbs = mRegion->fetchCollisionShapes(getRegion(), mBB, nullptr, false, nullptr);//&mTerrainSurfaceOffset
	AABB allIntersects = AABB::EMPTY;
	for(auto& it : preIntersectingAabbs) {
		if(allIntersects.isEmpty()) {
			allIntersects = it;
		}
		else {
			allIntersects.merge(it);
		}
	}

	if (mIsStuckInWeb) {
		mIsStuckInWeb = false;

		speed.x *= .25f;
		speed.y *= .05f;
		speed.z *= .25f;
		mPosDelta.x = .0f;
		mPosDelta.y = .0f;
		mPosDelta.z = .0f;
	}

	float xaOrg = speed.x;
	float yaOrg = speed.y;
	float zaOrg = speed.z;

	AABB bbOrg = mBB;

	bool sneaking = mOnGround && isSneaking();

	if (sneaking) {
		float d = 0.05f;
		
		//bias the step size a bit to make sure we reach the bottom of the step
		float maxUpStep = mMaxAutoStep * 1.01f;

		while (speed.x != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(speed.x, -maxUpStep, 0)), nullptr, false, nullptr).empty()) {
			if (speed.x < d && speed.x >= -d) {
				speed.x = 0;
			}
			else if (speed.x > 0) {
				speed.x -= d;
			}
			else {
				speed.x += d;
			}

			xaOrg = speed.x;
		}

		while (speed.z != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(0, -maxUpStep, speed.z)), nullptr, false, nullptr).empty()) {
			if (speed.z < d && speed.z >= -d) {
				speed.z = 0;
			}
			else if (speed.z > 0) {
				speed.z -= d;
			}
			else {
				speed.z += d;
			}

			zaOrg = speed.z;
		}

		while (speed.x != 0 && speed.z != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(speed.x, -maxUpStep, speed.z)), nullptr, false, nullptr).empty()) {
			if (speed.x < d && speed.x >= -d) {
				speed.x = 0;
			}
			else if (speed.x > 0) {
				speed.x -= d;
			}
			else {
				speed.x += d;
			}

			if (speed.z < d && speed.z >= -d) {
				speed.z = 0;
			}
			else if (speed.z > 0) {
				speed.z -= d;
			}
			else {
				speed.z += d;
			}

			xaOrg = speed.x;
			zaOrg = speed.z;
		}

	}
	Vec3 orgSpeed = speed;

	AABB intersectTestBox = mBB.expanded(speed);
	std::vector<AABB>& aABBs = mRegion->fetchCollisionShapes(getRegion(), intersectTestBox, nullptr, false, nullptr);

	//LOGE(
	//	"MOVE pos=(%f,%f,%f) speed=(%f,%f,%f) BB=(%f,%f,%f)->(%f,%f,%f) boxes=%zu\n",
	//	mPos.x, mPos.y, mPos.z,
	//	speed.x, speed.y, speed.z,
	//	mBB.min.x, mBB.min.y, mBB.min.z,
	//	mBB.max.x, mBB.max.y, mBB.max.z,
	//	aABBs.size()
	//);

	bool collidableWasNear = mCollidableMobNear;

	//	Because some entities can collide and interact differently with other entities (I.E. minecarts stacking) we want to go ahead and
	//	have the entity do it's own test for collision so we're not unnecessarily testing for collisions.
	/*if (isStackable()) {
		testForEntityStacking(*mRegion, intersectTestBox, aABBs);
	}

	else if (mCollidableMobNear) {
		testForCollidableMobs(*mRegion, intersectTestBox, aABBs);
		mCollidableMobNear = false;
	}*/

	// Use oneway collision for all blocks if the entity is within any oneway blocks, or they take advantage of one way physics intentionally,
	// or if they're stuck, to prevent thrashing around trying to resolve
	bool onewayCollide = false;//!mOnewayPhysicsBlocks.empty() || mUsesOneWayCollision || mStuckInCollider;

	// This is broken into 3 passes for the sake of smooth movement. Neighboring blocks on a smooth surface align the movement vector with the surface if possible
	// If all done at once, entities can catch on corners vertical edges while trying to move horizontally along a surface, or similarly when brushing up against walls
	Vec3 steps[3] = { Vec3(0.0f, speed.y, 0.0f), Vec3(speed.x, 0.0f, 0.0f), Vec3(0.0f, 0.0f, speed.z) };
	Vec3 penetration(0.0f);

	for (int j = 0; j < 3; ++j) {
		Vec3& step = steps[j];
		float& pen = penetration[j];
		for (auto& bb : aABBs) {
			float tempPenetration;
			step = bb.clipCollide(mBB, step, onewayCollide, &tempPenetration);
			pen = std::max(pen, tempPenetration);
		}
		moveBBs(step);
	}

	float totalPenetration = penetration.x + penetration.y + penetration.z;
	float epsilon = FLT_EPSILON;
	// If there was no penetration, then we're not stuck anymore
	if(mStuckInCollider && totalPenetration <= epsilon) {
		mStuckInCollider = false;
	}
	// if there was penetration last frame we might have fixed an error, but if it's also this frame, then we're probably stuck between two objects
	else if(!mStuckInCollider && mLastPenetration > epsilon && totalPenetration > epsilon) {
		mStuckInCollider = true;
	}
	mLastPenetration = totalPenetration;

	// Combine the collective move we just did back into here for later use
	speed = steps[0] + steps[1] + steps[2];
	bool og = mOnGround || (yaOrg != speed.y && yaOrg < 0);

	// If we ran into a vertical wall and we're on the ground
	if (mMaxAutoStep > 0 && og && ((xaOrg != speed.x) || (zaOrg != speed.z))) {
		// Try stepping up onto what's in front of us using our foot size, storing old velocity in case we can't do it
		// This is done by first trying to move up vertically, then over horizontally, in the hopes that the first move cleared the obstacle
		float xaN = speed.x;
		float yaN = speed.y;
		float zaN = speed.z;

		// Use the unadjusted speed before collision as we're trying again
		Vec3 speedX(xaOrg, 0.0f, 0.0f);
		Vec3 speedY(0.0f, mMaxAutoStep, 0.0f);
		Vec3 speedZ(0.0f, 0.0f, zaOrg);

		// Store original bounding box in case we want to revert
		AABB normal = mBB;
		std::vector<AABB> norm;
		for (auto& bb : mSubBBs) {
			norm.push_back(bb);
		}
		mBB.set(bbOrg);
		// Fetch collision shapes using original speed, this will exclude colliders above head, we'll check those at the end
		aABBs = mRegion->fetchCollisionShapes(getRegion(), mBB.expanded(orgSpeed), nullptr, true, nullptr);

		if (collidableWasNear) {
			//testForCollidableMobs(*mRegion, intersectTestBox, aABBs);
		}

		// Try to step up over the obstacle
		// Even though the motion is only on y, we don't want to use clipYCollide because that will force penetration resolution along the y axis, which may not be the smallest penetrating axis
		for (auto& bb : aABBs) {
			speedY = bb.clipCollide(mBB, speedY, onewayCollide);
		}
		moveBBs(speedY);

		// Now that we've attempted the vertical step, do the horizontal ones, which will hopefully not be impeded by the same obstacle as before
		for (auto& bb : aABBs) {
			speedX = bb.clipCollide(mBB, speedX, onewayCollide);
		}
		moveBBs(speedX);

		for (auto& bb : aABBs) {
			speedZ = bb.clipCollide(mBB, speedZ, onewayCollide);
		}
		moveBBs(speedZ);

		// Undo as much y axis movement as possible, we're climping up, not jumping up
		Vec3 reverseSpeedY = -speedY;
		for (auto& bb : aABBs) {
			reverseSpeedY = bb.clipCollide(mBB, reverseSpeedY, onewayCollide);
		}
		moveBBs(reverseSpeedY);
		speedY += reverseSpeedY;

		// Store the movement we just did in here for later use if we decide to keep it
		speed = speedX + speedY + speedZ;
		// See if we hit our head on the ceiling, meaning we shouldn't do step up.
		// This makes getting head stuck in ceiling possible if moving into a block lower than footsize at a speed larger than you bounding box width. This shouldn't be achievable by standard means
		// Not expanding bb by anything because we just want to see if we ended up in a good spot
		bool hitCeiling = !mRegion->fetchCollisionShapes(getRegion(), mBB, nullptr, true, nullptr).empty();

		// If our original horizontal square velocity was higher without trying to step over the obstacle, or our head ended up in the ceiling, revert back to that for the sake of smoother movement
		if (hitCeiling || xaN * xaN + zaN * zaN >= speed.x * speed.x + speed.z * speed.z) {
			speed.x = xaN;
			speed.y = yaN;
			speed.z = zaN;
			mBB.set(normal);
			for (size_t i = 0; i < mSubBBs.size(); ++i) {
				mSubBBs[i] = norm[i];
			}
		}
		// Else our vertical step cleared the obstacle and have more horizontal velocity than before, so keep the changes
		// Don't do mSlideOffset on client side except for players because it can cause tunneling for entities
		else if(true) {
			// Add an artificial downward offset that will ease up over time to smooth out the sudden change in height caused by this step
			mSlideOffset.y += speedY.y;
		}
	}

	{
		//ScopedProfile("rest");

		mPos.x = (mBB.min.x + mBB.max.x) / 2.0f;
		mPos.y = mBB.min.y + mHeightOffset - mSlideOffset.y;
		mPos.z = (mBB.min.z + mBB.max.z) / 2.0f;

		mHorizontalCollision = (xaOrg != speed.x) || (zaOrg != speed.z);
		mVerticalCollision = (yaOrg != speed.y);

		mOnGround = (yaOrg != speed.y && yaOrg < 0) || (mOnGround && yaOrg == speed.y && yaOrg == 0);
		mCollision = mHorizontalCollision || mVerticalCollision;

		//checkFallDamage(speed.y, mOnGround);

		if (xaOrg != speed.x) {
			mPosDelta.x = 0;
			//onBlockCollision(0);
		}

		if (yaOrg != speed.y) {
			/*auto stepPos = _getBlockOnPos();
			auto& onBlock = mRegion->getBlock(stepPos);

			onBlock.updateEntityAfterFallOn(*this);
			onBlockCollision(1);

			if (!onBlock.isType(Block::mAir) && mOnGround) {
				onBlock.onStepOn(*this, stepPos);
			}

			if (!sneaking) {
				Vec3 delta = mPos - mPosPrev;
				mWalkDist += Math::sqrt(delta.x * delta.x + delta.z * delta.z) * 0.6f;
			}*/
		}

		if (zaOrg != speed.z) {
			mPosDelta.z = 0;
			//onBlockCollision(2);
		}

		//_playMovementSound(mVerticalCollision);

		mSlideOffset.y *= 0.4f;

		//checkBlockCollisions();
	}
}


















void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	glinput.mouseXPos = xposIn;
	glinput.mouseYPos = yposIn;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	bool pressed = (action != GLFW_RELEASE);

	if (pressed != glinput.mlookup_table[button])
	{
		glinput.mlast_table[button] = pressed ? 1 : -1;
		glinput.mlookup_table[button] = pressed;
	}
}
//todo move more stuff to renderer init
GLFWwindow* renderThreadInit()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//3.3 minimum version (thats what this is linked with)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);//We need this for rendering
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenx, screeny, "Minecraft PE 1.0.0 Restoration", NULL, NULL);
	if (window == NULL)
	{
		DEBUG_FAIL("Failed to make GLFW window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		DEBUG_FAIL("Failed to init GLAD");
		glfwTerminate();
		return nullptr;
	}

	glViewport(0, 0, screenx, screeny); //this sets the screen size

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//Oh no the user was annoying and changed the window size. FIX IT

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide + lock cursor
	//glLineWidth(5.0f);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	return window;
}



//decompiled
void /*MinecraftClient::*/updateFoliageColors()
{
	mce::TextureGroup* textureGroup = &texgroup;//mTextures;

	auto foliage = textureGroup->getTexturePair(
		ResourceLocation("textures/colormap/foliage")
	);

	auto evergreen = textureGroup->getTexturePair(
		ResourceLocation("textures/colormap/evergreen")
	);

	auto birch = textureGroup->getTexturePair(
		ResourceLocation("textures/colormap/birch")
	);

	auto grass = textureGroup->getTexturePair(
		ResourceLocation("textures/colormap/grass")
	);

	FoliageColor::setFoliageColorPalette(*foliage.unwrap().getTextureData());

	FoliageColor::setFoliageEvergreenColorPalette(*evergreen.unwrap().getTextureData());

	FoliageColor::setFoliageBirchColorPalette(*birch.unwrap().getTextureData());

	FoliageColor::setGrassColorPalette(*grass.unwrap().getTextureData());
}

void renderThreadStuff()
{
	GLFWwindow* window = renderThreadInit();

	if (!window)
		return;

	bool cursor_locked = true;






	//RenderChunk rc[8]; (BlockPos(0, 0, 0));

	//RenderChunkRenderParameters renderParams;
	//renderParams.layer = TerrainLayer::Opaque;//?
	//renderParams.forceUnsorted = false;
	//renderParams.forceFog = false;

	mce::RenderMaterialGroup::switchable.InitRenderMaterials();

	//ShaderComponent mainVertex("./restore/shaders/test_textured.vs", VERTEX_SHADER);
	//ShaderComponent mainFragment("./restore/shaders/test_textured.fs", FRAGMENT_SHADER);

	//Shader shader(mainVertex, mainFragment);
	//shader.activate();
	//shader.setInt("atlas", 0);
	//glUniform1i(glGetUniformLocation(shader, "atlas"), 0);//set the atlas to be texture 0

	Camera c(commandContext.level,commandContext.dim);

	//sptr = &shader;



	ResourceLocation metaFile("textures/terrain_texture.json");

	ResourcePackManager resourcePackManager;

	auto atlas = make_shared<TextureAtlas>(metaFile, resourcePackManager, &texgroup);

	BlockGraphics::setTextureAtlas(atlas);

	atlas->loadMetaFile();

	BlockGraphics::initBlocks();
	RenderChunk::initMaterials(texgroup);

	updateFoliageColors();

	/*Tessellator::instance.begin(4);


	Tessellator::instance.vertexUV(0, 0, 0, 0.0f, 0.0f);
	Tessellator::instance.vertexUV(1, 0, 0, 1.0f, 0.0f);
	Tessellator::instance.vertexUV(1, 1, 0, 1.0f, 1.0f);
	Tessellator::instance.vertexUV(0, 1, 0, 0.0f, 1.0f);

	mce::Mesh m = Tessellator::instance.end();*/

	float currentFrame = 0.0f;
	float deltaTime;
	float lastFrame;


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
	};

	mce::MaterialPtr chunkMaterial = mce::RenderMaterialGroup::switchable.getMaterial("terrain_opaque");
	mce::MaterialPtr chunk2Material = mce::RenderMaterialGroup::switchable.getMaterial("terrain_water");
	mce::MaterialPtr chunk3Material = mce::RenderMaterialGroup::switchable.getMaterial("terrain_blend");
	
	std::vector<mce::MaterialPtr*> mptrs = { &chunkMaterial, &chunk2Material, &chunk3Material };

	glm::mat4 projection = glm::perspective(glm::radians(85.0f),
		(float)screenx / (float)screeny,
		0.1f, 1000.0f);



	for (auto sl : mptrs)
	{
		(*sl)->myShader->setMatrix4("PROJ", glm::value_ptr(projection));
		(*sl)->myShader->setInt("TEXTURE_0", 0);
		(*sl)->myShader->setFloat("FAR_CHUNKS_DISTANCE", 100);
		(*sl)->myShader->setFloat("RENDER_DISTANCE", 100);
	}

	std::unordered_map<ChunkPos,RenderChunk*[8]> rc;

	//std::unordered_set<ChunkPos> renderChunkMap;

	std::unordered_set<ChunkPos> needsReRendering;
	std::unordered_set<ChunkPos> needsReRendering2;

	std::unique_ptr chunkBuilder = make_unique<RenderChunkBuilder>(commandContext.dim->getChunkSource());



	while (!glfwWindowShouldClose(window)) 
	{
		lastFrame = currentFrame;
		currentFrame = glfwGetTime();

		deltaTime = currentFrame - lastFrame;
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//Set clear color to be brown with 0 alpha 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Actually clears screen + depth buffer 

		//run stuff here 
		c.UpdateCam(deltaTime);
		glm::mat4 view = glm::lookAt(c.mPos, c.mPos + c.cameraFront, c.cameraUp);
		
		//chunkMaterial->myShader->setVector4("CHUNK_ORIGIN_AND_SCALE", glm::vec4(0, 0, 0, 1));
		for (auto sl : mptrs)
		{
			(*sl)->myShader->setMatrix4("WORLDVIEW", glm::value_ptr(view));
		}

		ChunkPos camChunkPos(c.mPos.x/16,c.mPos.z/16);
		int renderDistance = 3;
		int loadDistance = renderDistance+1;//account for post-processing requiring chunks around it to be loaded

		

		for (int x = -loadDistance; x <= loadDistance; x++)
		{
			for (int z = -loadDistance; z <= loadDistance; z++)
			{
				ChunkPos cp(camChunkPos.x+x, camChunkPos.z+z);

				if (rc.find(cp) == rc.end())
				{
					commandContext.dim->getChunkSource().getOrLoadChunk(cp, ChunkSource::LoadMode::Deferred);
					needsReRendering.insert(cp);
					
				}
			}
		}



			for (auto it = needsReRendering.begin(); it != needsReRendering.end(); )
			{


				ChunkPos pos = *it;

				if (commandContext.dim->getChunkSource().getOrLoadChunk(pos, ChunkSource::LoadMode::None)->getState() == ChunkState::Loaded)
				{
					needsReRendering2.insert(pos);
					for (int i = 1; i < 9; i++)
					{
						if(commandContext.dim->getChunkSource().getOrLoadChunk(pos + offsets[i], ChunkSource::LoadMode::None)->getState() == ChunkState::Loaded)
							needsReRendering2.insert(pos+offsets[i]);
					}
					/*
					std::cout << "Rendering chunk at " << pos.x << ", " << pos.z << " ChunkPos\n";
					//int k = rc.size();
					for (int i = 0; i < 8; i++)
					{
						BlockPos cp(pos.x * 16, i * 16, pos.z * 16);




						RenderChunk* chunkPart = new RenderChunk(cp);

						chunkPart->startRebuild(std::move(chunkBuilder));
						chunkPart->rebuild(false, true);
						chunkBuilder = chunkPart->endRebuild(Vec3(c.mPos.x, c.mPos.y, c.mPos.z));
						//					chunkPart->startFaceSort(make_unique<RenderChunkSorter>());
						//chunkPart->faceSort();
						//chunkPart->endFaceSort();*
						rc[cp][i] = chunkPart;
					}*/
					it = needsReRendering.erase(it);
					//renderChunkMap.insert(pos);
				}
				else
				{
					it++;
				}
			}






			for (auto it = needsReRendering2.begin(); it != needsReRendering2.end(); )
			{
				ChunkPos pos = *it;

					std::cout << "Rendering chunk at " << pos.x << ", " << pos.z << " ChunkPos\n";
					//int k = rc.size();
					for (int i = 0; i < 8; i++)
					{
						BlockPos cp(pos.x * 16, i * 16, pos.z * 16);


						

						if(rc[cp][i] == nullptr)
							rc[cp][i] = new RenderChunk(cp);
						else
							rc[cp][i]->setDirty(Tick(0), true);

						RenderChunk* chunkPart = rc[cp][i];
						chunkPart->setDirty(Tick(0), true);

						chunkPart->startRebuild(std::move(chunkBuilder));
						chunkPart->rebuild(false, true);
						chunkBuilder = chunkPart->endRebuild(Vec3(c.mPos.x, c.mPos.y, c.mPos.z));
					}
					it = needsReRendering2.erase(it);
			}










		//m.render();
		for (auto & lc : rc)
		{
			for (int i = 0; i < 8; i++)
			{
				BlockPos position = lc.second[i]->getPosition();
				lc.second[i]->updateFaceSortState(Vec3(c.mPos.x, c.mPos.y, c.mPos.z));//
				if (lc.second[i]->isFaceSortDirty())
				{
					lc.second[i]->startFaceSort(make_unique<RenderChunkSorter>());
					lc.second[i]->faceSort();
					lc.second[i]->endFaceSort();
				}

				for (auto sl : mptrs)
				{
					(*sl)->myShader->setVector4("CHUNK_ORIGIN_AND_SCALE", glm::vec4(position.x, position.y, position.z, 1));
				}

				RenderChunkRenderParameters renderParams;
				renderParams.layer = TerrainLayer::Opaque;//Blend;//?
				renderParams.forceUnsorted = false;
				renderParams.forceFog = false;


				//shader.setMatrix4("model", glm::value_ptr(model));
				if (lc.second[i]->hasLayer(renderParams.layer))
					lc.second[i]->render(renderParams, currentFrame);
			}
		}

		for (auto& lc : rc)
		{
			for (int i = 0; i < 8; i++)
			{
				BlockPos position = lc.second[i]->getPosition();

				for (auto sl : mptrs)
				{
					(*sl)->myShader->setVector4("CHUNK_ORIGIN_AND_SCALE", glm::vec4(position.x, position.y, position.z, 1));
				}

				RenderChunkRenderParameters renderParams;
				renderParams.layer = TerrainLayer::Water;
				renderParams.forceUnsorted = false;
				renderParams.forceFog = false;


				//shader.setMatrix4("model", glm::value_ptr(model));
				if (lc.second[i]->hasLayer(renderParams.layer))
					lc.second[i]->render(renderParams, currentFrame);
			}
		}



		if (glinput.NewKeyDown(GLFW_KEY_U))
		{
			cursor_locked = !cursor_locked;
			glfwSetInputMode(window, GLFW_CURSOR, cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
		glinput.Clear();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	commandContext.running = false;
	glfwTerminate();
}