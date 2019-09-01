#pragma once

#include "Crystal/Renderer/Renderer.h"

namespace Crystal
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(void* Buffer, unsigned int Size, unsigned int Offset = 0) = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t size = 0);
	};
}