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
class Camera
{
public:
	glm::vec3 cameraPos = glm::vec3(0, 100, 0);
	glm::vec3 cameraFront = glm::vec3(0, -1, 0);
	glm::vec3 cameraUp = glm::vec3(0, 1, 0);

	double lastX;
	double lastY;
	bool firstMouse = true;
	double yaw = 0;
	double pitch = 0;

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


		float cameraSpeed = 5.0f * deltaTime;

		if (glinput.KeyDown(GLFW_KEY_LEFT_SHIFT) || glinput.KeyDown(GLFW_KEY_RIGHT_SHIFT))
			cameraSpeed *= 3;

		if (glinput.KeyDown(GLFW_KEY_W))
			cameraPos += cameraSpeed * cameraFront;
		if (glinput.KeyDown(GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glinput.KeyDown(GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glinput.KeyDown(GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
};


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

struct CCTX
{
	std::atomic<bool> running;
	Level* level;
	Dimension* dim;
};

extern CCTX commandContext;

mce::TextureGroup texgroup;

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

	RenderChunkRenderParameters renderParams;
	renderParams.layer = TerrainLayer::Opaque;//?
	renderParams.forceUnsorted = false;
	renderParams.forceFog = false;

	mce::RenderMaterialGroup::switchable.InitRenderMaterials();

	//ShaderComponent mainVertex("./restore/shaders/test_textured.vs", VERTEX_SHADER);
	//ShaderComponent mainFragment("./restore/shaders/test_textured.fs", FRAGMENT_SHADER);

	//Shader shader(mainVertex, mainFragment);
	//shader.activate();
	//shader.setInt("atlas", 0);
	//glUniform1i(glGetUniformLocation(shader, "atlas"), 0);//set the atlas to be texture 0

	Camera c;

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
	
	glm::mat4 projection = glm::perspective(glm::radians(85.0f),
		(float)screenx / (float)screeny,
		0.1f, 1000.0f);

	chunkMaterial->myShader->setMatrix4("PROJ", glm::value_ptr(projection));

	chunkMaterial->myShader->setInt("TEXTURE_0", 0);

	std::unordered_map<ChunkPos,RenderChunk*[8]> rc;

	//std::unordered_set<ChunkPos> renderChunkMap;

	std::unordered_set<ChunkPos> needsReRendering;

	while (!glfwWindowShouldClose(window)) 
	{
		lastFrame = currentFrame;
		currentFrame = glfwGetTime();

		deltaTime = currentFrame - lastFrame;
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//Set clear color to be brown with 0 alpha 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Actually clears screen + depth buffer 

		//run stuff here 
		c.UpdateCam(deltaTime);
		glm::mat4 view = glm::lookAt(c.cameraPos, c.cameraPos + c.cameraFront, c.cameraUp);
		
		//chunkMaterial->myShader->setVector4("CHUNK_ORIGIN_AND_SCALE", glm::vec4(0, 0, 0, 1));
		chunkMaterial->myShader->setMatrix4("WORLDVIEW", glm::value_ptr(view));

		ChunkPos camChunkPos(c.cameraPos.x/16,c.cameraPos.z/16);
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
					std::cout << "Rendering chunk at " << pos.x << ", " << pos.z << " ChunkPos\n";
					//int k = rc.size();
					for (int i = 0; i < 8; i++)
					{
						BlockPos cp(pos.x * 16, i * 16, pos.z * 16);




						RenderChunk* chunkPart = new RenderChunk(cp);

						chunkPart->startRebuild(make_unique<RenderChunkBuilder>(commandContext.dim->getChunkSource()));
						chunkPart->rebuild(false, true);
						chunkPart->endRebuild(Vec3(c.cameraPos.x, c.cameraPos.y, c.cameraPos.z));
						/*					chunkPart->startFaceSort(make_unique<RenderChunkSorter>());
						chunkPart->faceSort();
						chunkPart->endFaceSort();*/
						rc[cp][i] = chunkPart;
					}
					it = needsReRendering.erase(it);
					//renderChunkMap.insert(pos);
				}
				else
				{
					it++;
				}
			}

		//m.render();
		for (auto & lc : rc)
		{
			for (int i = 0; i < 8; i++)
			{
				//glm::mat4 model(1);

				BlockPos position = lc.second[i]->getPosition();

				//model = glm::translate(model, glm::vec3(position.x, position.y, position.z));

				//glm::mat4 mv = model * view;


				//
				lc.second[i]->updateFaceSortState(Vec3(c.cameraPos.x, c.cameraPos.y, c.cameraPos.z));//
				if (lc.second[i]->isFaceSortDirty())
				{
					lc.second[i]->startFaceSort(make_unique<RenderChunkSorter>());
					lc.second[i]->faceSort();
					lc.second[i]->endFaceSort();
				}
				
				chunkMaterial->myShader->setVector4("CHUNK_ORIGIN_AND_SCALE", glm::vec4(position.x, position.y, position.z, 1));
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