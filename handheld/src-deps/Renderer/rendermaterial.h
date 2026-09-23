//tac todo
#pragma once
#include <string>
#include "json/json.h"
namespace mce
{
	class RS
	{

	};

	class RenderMaterial
	{
	public:
		static void initContext();
		RenderMaterial& operator=(RenderMaterial const&);
	protected: 
		std::string _buildHeader(Json::Value const&);
		RS _parseStateName(std::string const&);
	void SpliceShaderPath(std::string &);
	};
}