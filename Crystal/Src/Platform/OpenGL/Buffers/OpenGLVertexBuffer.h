#pragma once

#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/Buffers/VertexBuffer.h"
#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(unsigned int Size);
		virtual ~OpenGLVertexBuffer();

		virtual void SetData(void* Buffer, unsigned int Size, unsigned int Offset = 0) override;

		virtual void Bind() const override;
		virtual void UnBind() const override;

		inline virtual const BufferLayout& GetLayout() const override { return mLayout; }
		virtual void SetLayout(const BufferLayout& layout) override { mLayout = layout; }

	private:
		RendererID mRendererID;
		unsigned int mSize;

		BufferLayout mLayout;
	};
}
