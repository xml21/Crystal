#pragma once
#include "BufferLayout.h"

namespace Crystal
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		//TODO: Consider adding smart pointer for reference counting
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};
}
