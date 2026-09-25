#include "Mesh.h"
#include "TexturePtr.h"
#include "hal/interface/RenderContextImmediate.h"
#include "materialptr.h"
#include <glad/glad.h>
namespace mce
{

    int fieldSizes[] =
    {
        sizeof(float) * 3,
        sizeof(unsigned int),
        sizeof(int),
        sizeof(unsigned short) * 2,
        sizeof(unsigned short) * 2,
        sizeof(unsigned short) * 2,
    };

    int fieldTypes[] =
    {
        GL_FLOAT,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_INT,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_SHORT,
    };

    int primitiveConversions[] =
    {
        0,
        GL_QUADS,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_LINES,
        GL_LINE_STRIP
    };

    bool doNormalize[] =
    {
        false,
        true,
        false,
        true,//we want our uvs to be converted back to 0-1
        true,
        true,
    };

    int fieldAmounts[] =
    {
        3,
        4,
        1,
        2,
        2,
        2,
    };

    const char* vertexFormatNames[] =
    {
        "POSITION",
        "COLOR",
        "NORMAL",
        "TEXCOORD_0",
        "TEXCOORD_1",
        "TEXCOORD_2",
    };

    VertexFormat VertexFormat::EMPTY;

    void VertexFormat::uploadToVAO(unsigned int VAO)
    {


            glBindVertexArray(VAO);

            int offs = 0;

            int vSize = getVertexSize();

            for (int i = 0; i < (int)VertexField::NumVertexFields; i++)
            {
                if (useFields[i])
                {
                    glEnableVertexAttribArray(i);
                    //quick-ish hack
                    if(fieldTypes[i] == GL_FLOAT || fieldTypes[i] == GL_HALF_FLOAT || doNormalize[i])
                    glVertexAttribPointer(i, fieldAmounts[i], fieldTypes[i], doNormalize[i], vSize, (void*)offs);
                    else
                        glVertexAttribIPointer(i, fieldAmounts[i], fieldTypes[i], vSize, (void*)offs);
                    offs += fieldSizes[i];
                }
            }
            glBindVertexArray(0);
    }

	void Mesh::upload()
	{
        glGenVertexArrays(1, &VAO);

        

        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);


        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        if (indexCount * indexSize > 0)
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, byteData + (vertexCount * vertexSize), GL_STATIC_DRAW);
        }

        glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, byteData, GL_STATIC_DRAW);


        vf.uploadToVAO(VAO);//apparently this doesnt work if you put it at the start (setting up the VAO also binds the vbo etc to it)


        glBindVertexArray(0);
	}

    void Mesh::render()
    {
        if (VBO == -1)
            upload();

        glBindVertexArray(VAO);
       

        if (useMode == IndexBufferUsageMode::Normal && indexCount * indexSize > 0)
        {
            int realMode = mode == PrimitiveMode::QuadList ? GL_TRIANGLES : primitiveConversions[(int)mode];
            glDrawElements(realMode, indexCount, indexSize == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR)
                printf("Before draw: %x\n", err);

            glDrawArrays(primitiveConversions[(int)mode], 0, vertexCount);

            while ((err = glGetError()) != GL_NO_ERROR)
                printf("After draw: %x\n", err);

           // glDrawArrays(primitiveConversions[(int)mode], 0, vertexCount);
        }
        glBindVertexArray(0);
    }

    void Mesh::_move(Mesh & other)
    {
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        other.VAO = -1;
        other.VBO = -1;
        other.EBO = -1;
        vf = other.vf;
        vertexCount = other.vertexCount;
        vertexSize = other.vertexSize;
        indexCount = other.indexCount;
        indexSize = other.indexSize;
        mode = other.mode;
        byteData = other.byteData;
    }

    Mesh& Mesh::operator=(Mesh&& lll)
    {
        _move(lll);
        return *this;
    }

    void Mesh::render(const mce::MaterialPtr& mat)
    {
        mat->activate();
        render();
    }

    void Mesh::render(const mce::MaterialPtr& mat, const mce::TexturePtr& text1)
    {
        if ((*text1) != nullptr)
        text1->bindTexture(RenderContextImmediate::get(), 0, 0);
        render(mat);
    }

    void Mesh::render(const mce::MaterialPtr& mat, const mce::TexturePtr& text, const mce::TexturePtr& text2)
    {
        if ((*text2) != nullptr)
        text2->bindTexture(RenderContextImmediate::get(), 1, 0);
        render(mat,text);
    }

    void Mesh::render(const mce::MaterialPtr& mat, const mce::TexturePtr& text1, const mce::TexturePtr&text2, const mce::TexturePtr&text3, int start, int count)
    {
        if((*text3) != nullptr)
        text3->bindTexture(RenderContextImmediate::get(), 2, 0);
        if ((*text2) != nullptr)
        text2->bindTexture(RenderContextImmediate::get(), 1, 0);
        if ((*text1) != nullptr)
        text1->bindTexture(RenderContextImmediate::get(), 0, 0);
        mat->activate();
        if (VBO == -1)
            upload();

        glBindVertexArray(VAO);


        if (useMode == IndexBufferUsageMode::Normal && indexCount * indexSize > 0)
        {
            //printf("start %i count %i\n", start, count);
            int realMode = mode == PrimitiveMode::QuadList ? GL_TRIANGLES : primitiveConversions[(int)mode];//hack because for some reason sorting changes this to tris not gl_quad (maybe non-sorted wasnt updated from immediate mode gl?)
            glDrawElements(realMode, count, indexSize == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, (void *)(start*indexSize));//apparently this is in bytes not in units of indices
        }
        else
        {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR)
                printf("Before draw: %x\n", err);

            

            glDrawArrays(primitiveConversions[(int)mode], 0, vertexCount);

            while ((err = glGetError()) != GL_NO_ERROR)
                printf("After draw: %x\n", err);

            // glDrawArrays(primitiveConversions[(int)mode], 0, vertexCount);
        }
        glBindVertexArray(0);
    }

    void Mesh::reset()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        VAO = -1;
        VBO = -1;
        EBO = -1;

        vf = VertexFormat::EMPTY;
        vertexCount = 0;
        vertexSize = 0;
        indexCount = 0;
        indexSize = 0;
        mode = PrimitiveMode::QuadList;
        byteData = nullptr;
    }

    Mesh::~Mesh()
    {
        reset();
    }


    void Mesh::loadIndexBuffer(uint32_t _indexSize, void const* data, unsigned int count)
    {
        indexSize = _indexSize;
        indexCount = count;
        glBindVertexArray(VAO);
        if (EBO != -1)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                count*indexSize,
                data,
                GL_DYNAMIC_DRAW
            );
        }
        else
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, GL_STATIC_DRAW);
        }
    }

};