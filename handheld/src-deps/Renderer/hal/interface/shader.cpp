#define NOMINMAX
#include "shader.h"
#include <Resource/ResourceHelper.h>

#include "Debug/Log.h"
#include <fstream>
#include <iostream>



namespace mce
{
	Shader::Shader(const std::string& fragmentFile, const std::string& vertexFile, const std::vector<std::string>& vertexFields, const std::vector<std::string>& defines)
	{

		GLuint fragment = doComponent(GL_FRAGMENT_SHADER, fragmentFile, defines);
		GLuint vertex = doComponent(GL_VERTEX_SHADER, vertexFile, defines);
        if (fragment != -1 && vertex != -1)
        {
            shaderid = glCreateProgram();
            glAttachShader(shaderid, vertex);
            glAttachShader(shaderid, fragment);



            for (int i = 0; i < (int)VertexField::NumVertexFields; i++)
            {
                //Do i need to only use the vertexfields ones? idk
                glBindAttribLocation(shaderid, i, vertexFormatNames[i]);//make sure they are at the right positions
            }

            glLinkProgram(shaderid);

            int  success;
            glGetProgramiv(shaderid, GL_LINK_STATUS, &success);

            if (!success)
            {
                char infoLog[2048];
                glGetProgramInfoLog(shaderid, 2048, NULL, infoLog);
                LOGE("Shader link error \n%s\n", infoLog);
                exit(0);
            }

            glDeleteShader(fragment);
            glDeleteShader(vertex);
        }

	}

    bool evalShader(const std::string& file, std::string& output, int &oOffset)
    {
        ResourceLocation rl(file);

        std::string input;

        Resource::load(rl, input);

        int inputOffset = 0;
        //int outputOffset = 0;

        while (true)
        {
            size_t incoffset = input.find("#include", inputOffset);//todo check for commented ones

            if (incoffset == std::string::npos)
            {
                output.insert(
                    output.begin() + oOffset,
                    input.begin() + inputOffset,
                    input.end()
                );

                oOffset += static_cast<int>(input.size() - inputOffset);
                break;
            }

            output.insert(output.begin() + oOffset, input.begin()+inputOffset, input.begin() + incoffset);
            oOffset += (incoffset - inputOffset);

            int offset = incoffset + 8;

            int startSingle = input.find("\'", offset);
            int startDouble = input.find("\"", offset);

            
            bool isSingleQ = ((startSingle < startDouble) && (startSingle != std::string::npos));
            int startNewName = (isSingleQ ? startSingle : startDouble)+1;


            if (startNewName == (std::string::npos+1))//its negative, not found
            {
                assert(0);
                return false;
            }
            else
            {
                int endNewName = isSingleQ ? input.find("\'", startNewName) : input.find("\"", startNewName);

                if (endNewName == std::string::npos)
                {
                    assert(0);
                    return false;
                }

                bool result = evalShader(input.substr(startNewName, endNewName - startNewName),output,oOffset);
                if (result == false)
                    return false;
                inputOffset = endNewName + 1;
            }

        }
        return true;
    }

    

	GLuint Shader::doComponent(unsigned int type, const std::string& componentfile, const std::vector<std::string>& defines)
	{
       
        std::string output;

        int tempoffset = 0;
        if (!evalShader(componentfile, output, tempoffset))
        {
            LOGE("Malformated shader include parsing error detected\n");
            return -1;
        }

        std::vector<std::string> hhh;
        std::vector<const char*> cc;


        int offset = output.find("// __multiversion__");
        if (offset == 0)
            hhh.push_back("#version 330 core\n");//do i need compatibility?

        

        for (auto& define : defines)
        {
            hhh.push_back(std::string("#define ") + define + "\n");
        }

        hhh.push_back("#define POS4 vec4\n#define POS3 vec3\n#define MAT4 mat4\n");

        hhh.push_back(output);

        for (auto& h : hhh)
        {
            cc.push_back(h.c_str());
        }

        /*static int rwr = 0;

        std::ofstream outputff("./resource_packs/vanilla/" + componentfile + std::to_string(rwr) + ".txt");
        rwr++;

        for (auto& h : hhh)
        {
            outputff << h;
            cc.push_back(h.c_str());
        }
        outputff.close();*/

        LOGI("Compiling shader %s\n", componentfile.c_str());


		GLuint shaderComponent = glCreateShader(type);

		glShaderSource(shaderComponent, cc.size(), cc.data(), NULL);
		glCompileShader(shaderComponent);

		int  success;

		glGetShaderiv(shaderComponent, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			char infoLog[2048];
			glGetShaderInfoLog(shaderComponent, 2048, NULL, infoLog);
			LOGE("Shader compile error for %s\n%s\n", componentfile.c_str(), infoLog);

            for (auto& h : hhh)
            {
                std::cout << h;
            }
            assert(0);

			return -1;
		}


		return shaderComponent;
	}






    Shader::~Shader()
    {
        glDeleteProgram(shaderid);
    }

    void Shader::activate() const
    {
        glUseProgram(shaderid);
    }

    void Shader::setMatrix2(const std::string& name, float* value) const
    {
        activate();
        unsigned int projLoc = glGetUniformLocation(shaderid, name.c_str());
        glUniformMatrix2fv(projLoc, 1, GL_FALSE, value);
    }

    void Shader::setMatrix3(const std::string& name, float* value) const
    {
        activate();
        unsigned int projLoc = glGetUniformLocation(shaderid, name.c_str());
        glUniformMatrix3fv(projLoc, 1, GL_FALSE, value);
    }

    void Shader::setMatrix4(const std::string& name, float* value) const
    {
        activate();
        unsigned int projLoc = glGetUniformLocation(shaderid, name.c_str());
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, value);
    }

    void Shader::setBool(const std::string& name, bool value) const
    {
        activate();
        glUniform1i(glGetUniformLocation(shaderid, name.c_str()), value);
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        activate();
        glUniform1i(glGetUniformLocation(shaderid, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        activate();
        glUniform1f(glGetUniformLocation(shaderid, name.c_str()), value);
    }

    void Shader::setVector2(const std::string& name, float x, float y) const
    {
        activate();
        glUniform2f(glGetUniformLocation(shaderid, name.c_str()), x, y);
    }

    void Shader::setVector2(const std::string& name, glm::vec2& vec) const
    {
        activate();
        glUniform2f(glGetUniformLocation(shaderid, name.c_str()), vec.x, vec.y);
    }

    void Shader::setVector3(const std::string& name, float x, float y, float z) const
    {
        activate();
        glUniform3f(glGetUniformLocation(shaderid, name.c_str()), x, y, z);
    }

    void Shader::setVector3(const std::string& name, glm::vec3& vec) const
    {
        activate();
        glUniform3f(glGetUniformLocation(shaderid, name.c_str()), vec.x, vec.y, vec.z);
    }

    void Shader::setVector4(const std::string& name, float x, float y, float z, float w) const
    {
        activate();
        glUniform4f(glGetUniformLocation(shaderid, name.c_str()), x, y, z, w);
    }

    void Shader::setVector4(const std::string& name, glm::vec4& vec) const
    {
        activate();
        glUniform4f(glGetUniformLocation(shaderid, name.c_str()), vec.x, vec.y, vec.z, vec.w);
    }

    void Shader::setMatrix2(unsigned int offset, float* value) const
    {
        activate();
        glUniformMatrix2fv(offset, 1, GL_FALSE, value);
    }

    void Shader::setMatrix3(unsigned int offset, float* value) const
    {
        activate();
        glUniformMatrix3fv(offset, 1, GL_FALSE, value);
    }

    void Shader::setMatrix4(unsigned int offset, float* value) const
    {
        activate();
        glUniformMatrix4fv(offset, 1, GL_FALSE, value);
    }

    void Shader::setBool(unsigned int offset, bool value) const
    {
        activate();
        glUniform1i(offset, value);
    }

    void Shader::setInt(unsigned int offset, int value) const
    {
        activate();
        glUniform1i(offset, value);
    }

    void Shader::setFloat(unsigned int offset, float value) const
    {
        activate();
        glUniform1f(offset, value);
    }

    void Shader::setVector2(unsigned int offset, float x, float y) const
    {
        activate();
        glUniform2f(offset, x, y);
    }

    void Shader::setVector2(unsigned int offset, glm::vec2& vec) const
    {
        activate();
        glUniform2f(offset, vec.x, vec.y);
    }

    void Shader::setVector3(unsigned int offset, float x, float y, float z) const
    {
        activate();
        glUniform3f(offset, x, y, z);
    }

    void Shader::setVector3(unsigned int offset, glm::vec3& vec) const
    {
        activate();
        glUniform3f(offset, vec.x, vec.y, vec.z);
    }

    void Shader::setVector4(unsigned int offset, float x, float y, float z, float w) const
    {
        activate();
        glUniform4f(offset, x, y, z, w);
    }

    void Shader::setVector4(unsigned int offset, glm::vec4& vec) const
    {
        activate();
        glUniform4f(offset, vec.x, vec.y, vec.z, vec.w);
    }


};