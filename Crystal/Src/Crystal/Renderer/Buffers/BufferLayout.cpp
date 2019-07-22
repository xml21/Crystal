#include "Crpch.h"
#include "BufferLayout.h"

namespace Crystal
{

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : mElements(elements)
	{
		CalculateOffsetAndStride();
	}

	void BufferLayout::CalculateOffsetAndStride()
	{
		mStride = 0;

		for (auto& Element : mElements)
		{
			Element.Offset = mStride;
			mStride += Element.Size;
		}
	}
}