#pragma once
#include "BufferLayout.h"

#include "Crystal/Renderer/Renderer.h"

namespace Crystal
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(void* Buffer, unsigned int Count, unsigned int Offset = 0) = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const= 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(uint32_t size = 0);
	};
}
