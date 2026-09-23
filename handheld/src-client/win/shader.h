#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <unordered_map>

enum ShaderType
{
	FRAGMENT_SHADER,
	VERTEX_SHADER
};

class ShaderComponent
{
public:
	ShaderComponent(const char * path, ShaderType type);
	ShaderComponent(const std::vector<std::string>& chunks, ShaderType type);
	~ShaderComponent();

	unsigned int shaderComponent;

	ShaderType type;
};

class Shader 
{
public:
	unsigned int id;
	Shader(const ShaderComponent& vertex, const ShaderComponent& fragment);
	~Shader();
	void activate() const;

	void setMatrix2(const std::string& name, float*) const;
	void setMatrix3(const std::string& name, float *) const;
	void setMatrix4(const std::string& name, float*) const;
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVector2(const std::string& name, float valuex, float valuey) const;
	void setVector2(const std::string& name, glm::vec2& vector) const;
	void setVector3(const std::string& name, float valuex, float valuey,float valuez) const;
	void setVector3(const std::string& name, glm::vec3 &vector) const;
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