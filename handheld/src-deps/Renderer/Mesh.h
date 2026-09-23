//tac mostly done
#pragma once
#include "VertexFormat.h"
#include <cstdint>
#include "Core/Utility/PrimitiveTypes.h"
#include "hal/enums/PrimitiveMode.h"

namespace mce
{
	class MaterialPtr;
	class TexturePtr;

	enum class IndexBufferUsageMode
	{
		Normal,
		ForceUseUnsorted,
	};


	class Mesh
	{
	public:
		VertexFormat vf;
		int vertexCount;
		int vertexSize;
		int indexCount;
		int indexSize;
		PrimitiveMode mode = PrimitiveMode::QuadList;
		byte* byteData = nullptr;

		unsigned int VAO = -1;
		unsigned int VBO = -1;
		unsigned int EBO = -1;

		void _move(Mesh& other);
		Mesh& operator=(Mesh&& lll);

		Mesh(Mesh && other)
		{
			_move(other);
		}

		Mesh()
		{
			vertexSize = 0;
			vertexCount = 0;
			indexCount = 0;
			indexSize = 0;
		}
		Mesh(VertexFormat _vertexFormat,unsigned int _vertexCount,int _indexCount, int _indexSize, PrimitiveMode _mode,byte* _data,bool _dontUpload)
		{
			vf = _vertexFormat;
			vertexCount = _vertexCount;
			vertexSize = vf.getVertexSize();
			indexCount = _indexCount;
			indexSize = _indexSize;
			mode = _mode;
			byteData = _data;
			if (!_dontUpload)
				upload();
		}

		~Mesh();

		void upload();

		void render();

		void render(const mce::MaterialPtr& mat);

		void render(const mce::MaterialPtr& mat, const mce::TexturePtr&);

		void render(const mce::MaterialPtr& mat, const mce::TexturePtr&, const mce::TexturePtr&);

		void render(const mce::MaterialPtr& mat, const mce::TexturePtr&, const mce::TexturePtr&, const mce::TexturePtr&, int, int);

		void setIndexBufferUsageMode(IndexBufferUsageMode)
		{

		}

		void loadIndexBuffer(uint32_t _indexSize, void const* data, unsigned int count);

		void reset();
	};
}