#pragma once
//TAC restored

namespace mce
{
	enum class VertexField
	{
		Position,
		Color,
		Normal,
		UV0,
		UV1,
		UV2,
		NumVertexFields,
	};

	const char* vertexFormatNames[];

	int fieldSizes[];

	class VertexFormat
	{
	public:
		void uploadToVAO(unsigned int);

		bool hasField(VertexField vf) const
		{
			return useFields[(int)vf];
		}
		int getFieldOffset(VertexField vf) const
		{
			int offs = 0;
			for (int i = 0; i < (int)vf; i++)
			{
				if(useFields[i])
				offs += fieldSizes[i];
			}
			return offs;
		}
		constexpr int getVertexSize() const
		{
			int offs = 0;
			for (int i = 0; i < (int)VertexField::NumVertexFields; i++)
			{
				if(useFields[i])
				offs += fieldSizes[i];
			}
			return offs;
		}
		void enableField(VertexField vf)
		{
			useFields[(int)vf] = true;
		}

		bool useFields[(int)VertexField::NumVertexFields];

		static VertexFormat EMPTY;
	};
}