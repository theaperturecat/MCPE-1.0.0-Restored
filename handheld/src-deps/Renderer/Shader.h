#pragma once
#include <glad/glad.h>
#include "VertexFormat.h"
//#include <Debug/Log.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace mce {
	struct Shader {


		unsigned int shaderid;

		Shader(const std::string& fragmentFile, const std::string& vertexFile, const std::vector<std::string>& vertexFields, const std::vector<std::string>& defines);
		~Shader();


		GLuint doComponent(unsigned int type, const std::string& componentfile, const std::vector<std::string>& defines);

		void activate() const;

		void setMatrix2(const std::string& name, float*) const;
		void setMatrix3(const std::string& name, float*) const;
		void setMatrix4(const std::string& name, float*) const;
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVector2(const std::string& name, float valuex, float valuey) const;
		void setVector2(const std::string& name, glm::vec2& vector) const;
		void setVector3(const std::string& name, float valuex, float valuey, float valuez) const;
		void setVector3(const std::string& name, glm::vec3& vector) const;
		void setVector4(const std::string& name, float valuex, float valuey, float valuez, float valuew) const;
		void setVector4(const std::string& name, glm::vec4& vector) const;

		void setMatrix2(unsigned int offset, float*) const;
		void setMatrix3(unsigned int offset, float*) const;
		void setMatrix4(unsigned int offset, float*) const;
		void setBool(unsigned int offset, bool value) const;
		void setInt(unsigned int offset, int value) const;
		void setFloat(unsigned int offset, float value) const;
		void setVector2(unsigned int offset, float valuex, float valuey) const;
		void setVector2(unsigned int offset, glm::vec2& vector) const;
		void setVector3(unsigned int offset, float valuex, float valuey, float valuez) const;
		void setVector3(unsigned int offset, glm::vec3& vector) const;
		void setVector4(unsigned int offset, float valuex, float valuey, float valuez, float valuew) const;
		void setVector4(unsigned int offset, glm::vec4& vector) const;

	};
}