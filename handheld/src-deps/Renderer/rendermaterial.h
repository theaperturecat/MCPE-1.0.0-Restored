//tac todo
#pragma once
#include <string>
#include "json/json.h"
#include "Shader.h"
namespace mce
{
	class RS
	{

	};

	class RenderMaterial
	{
	public:
		RenderMaterial(Json::Value&, std::string = "");//, mce::RenderMaterial* = nullptr);
		//static void initContext();
		RenderMaterial& operator=(RenderMaterial const&);

		Json::Value getNodeValue(const std::string &);
		void init();

		RenderMaterial* parent = nullptr;
		std::string parentName;

		void activate() const;

		Shader* myShader;

	protected: 
		//std::string _buildHeader(Json::Value const&);
		//RS _parseStateName(std::string const&);
	//void SpliceShaderPath(std::string &);


		Json::Value myroot;


		/*      "InvertCulling",
      "DisableDepthWrite",
      "EnableStencilTest",
      "StencilWrite"
	  "Blending"
	  "DisableCulling"*/

		enum class CullMode
		{
			NORMAL,
			INVERT,
			DISABLED,
		} cullMode = CullMode::NORMAL;
		unsigned int depthFunc = GL_LESS;
		bool depthWrite = true;
		bool stencilTest = false;
		bool stencilWrite = false;
		bool blending = false;

	};
}