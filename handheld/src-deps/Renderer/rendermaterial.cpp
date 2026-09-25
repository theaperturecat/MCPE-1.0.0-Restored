#include "rendermaterial.h"

#include "Debug/Log.h"

namespace mce
{

	RenderMaterial::RenderMaterial(Json::Value& root,std::string _parent)//, mce::RenderMaterial* _parent)
	{
		myroot = root;
		parentName = _parent;
		//parent = _parent;
	}



	void RenderMaterial::init()
	{
		std::string vertexPath = getNodeValue("vertexShader").asString();
		std::string fragmentPath = getNodeValue("fragmentShader").asString();

		std::vector<std::string> defines;

		for (auto& n : getNodeValue("defines"))
		{
			defines.push_back(n.asString());
		}

		//std::vector<std::string> defines = getNodeValue("defines")//getNodeValue("defines").getMemberNames();
		std::vector<std::string> vertexFields;
		Json::Value vfields = getNodeValue("vertexFields");
		for (auto& n : vfields)
		{
			vertexFields.push_back(n["field"].asString());
		}
		myShader = new Shader(fragmentPath,vertexPath,vertexFields,defines);

		for (auto& str : getNodeValue("states"))
		{
			if (str == "InvertCulling")
			{
				cullMode = CullMode::INVERT;
			}
			else if (str == "DisableCulling")
			{
				cullMode = CullMode::DISABLED;
			}
			else if (str == "Blending")
			{
				blending = true;
			}
			else if (str == "EnableStencilTest")
			{
				stencilTest = true;
			}
			else if (str == "DisableDepthWrite")
			{
				depthWrite = false;
			}
			else if (str == "StencilWrite")
			{
				stencilWrite = true;
			}
			else if (str == "DisableAlphaWrite")
			{

			}
			else if (str == "DisableColorWrite")
			{

			}
			else if (str == "EnableAlphaToCoverage")
			{
				//?
			}
			else
			{
				LOGW("Unknown material state %s\n",str.asCString());
				assert(0);
			}
		}
		Json::Value dfunc = getNodeValue("depthFunc");
		if (!dfunc.isNull())
		{
			std::string dfunc2 = dfunc.asString();
			if (dfunc2 == "LessEqual")
			{
				depthFunc = GL_LEQUAL;
			}
			else if (dfunc2 == "Less")
			{
				depthFunc = GL_LESS;
			}
			else if (dfunc2 == "Greater")
			{
				depthFunc = GL_GREATER;
			}
			else if (dfunc2 == "GreaterEqual")
			{
				depthFunc = GL_GEQUAL;
			}
			else if (dfunc2 == "Equal")
			{
				depthFunc = GL_EQUAL;
			}
			else if (dfunc2 == "NotEqual")
			{
				depthFunc = GL_NOTEQUAL;
			}
			else if (dfunc2 == "Always")
			{
				depthFunc = GL_ALWAYS;
			}
			else if (dfunc2 == "Never")
			{
				depthFunc = GL_NEVER;
			}
			else
			{
				LOGW("Unknown material depthFunc %s\n", dfunc2.c_str());
				assert(0);
			}
		}

	}

	Json::Value RenderMaterial::getNodeValue(const std::string & arg)
	{
		Json::Value val = myroot.get(arg, Json::Value());
		if(val.isNull() && parent)
			val = parent->getNodeValue(arg);
		return val;
	}

	void RenderMaterial::activate() const
	{
		/*if (depthWrite)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}
		glDepthFunc(depthFunc);
		if (blending)
		{
			//glEnable(GL_BLEND);
		}
		else
		{
			//glDisable(GL_BLEND);
		}

		if (cullMode == CullMode::DISABLED)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(cullMode == CullMode::NORMAL ? GL_CW : GL_CCW);
		}*/

		myShader->activate();
	}
};