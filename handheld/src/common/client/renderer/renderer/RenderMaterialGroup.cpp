#include "common_header.h"
#include "materialptr.h"
#include <string>
#include <Resource/ResourceHelper.h>
#include <json/json.h>
#include <cassert>
#include "RenderMaterialGroup.h"
#include <Debug/Log.h>
namespace mce
{
	void RenderMaterialGroup::LoadRenderMaterialFile(std::string rr)
	{
		ResourceLocation rl(rr);

		std::string parseString;
		Json::Value root;
		Json::Reader reader;

		Resource::load(rl, parseString);
		bool parsingSucessful = reader.parse(parseString, root);

		for (auto& nodeName : root.getMemberNames())
		{
			int parentSplitIndex = nodeName.find(":");
			if (parentSplitIndex != -1)
			{
				std::string myName = nodeName.substr(0, parentSplitIndex);
				std::string myParent = nodeName.substr(parentSplitIndex+1, nodeName.size()-(parentSplitIndex+1));

				//if (materials.find(myParent) == materials.end())
				//{
				//	LOGE("Missing parent material\n");
				//}

				materials[myName] = new RenderMaterial(root[nodeName], myParent);// , materials[myParent]);
			}
			else
			{
				materials[nodeName] = new RenderMaterial(root[nodeName]);
			}
			std::cout << nodeName << '\n';
		}
	}

	void RenderMaterialGroup::InitRenderMaterials()
	{
		ResourceLocation rl("materials/sad.json");


		std::string parseString;
		Json::Value root;
		Json::Reader reader;

		Resource::load(rl, parseString);


		bool parsingSucessful = reader.parse(parseString, root);
		if (!parsingSucessful) {
			return;
		}

		assert(root.isArray());

		for (const auto& item : root)
		{
			std::string materialPath = item["path"].asString();
			LoadRenderMaterialFile(materialPath);
		}

		for (auto& [k, m] : materials)
		{
			if(m->parentName != "")
			m->parent = materials[m->parentName];
		}
		for (auto& [k, m] : materials)
		{
			m->init();
		}

	}
}