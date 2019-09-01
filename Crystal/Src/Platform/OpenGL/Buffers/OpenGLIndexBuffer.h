#pragma once

#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int Size);
		virtual ~OpenGLIndexBuffer();

		virtual void SetData(void* Buffer, unsigned int mSize, unsigned int Offset = 0) override;

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual uint32_t GetCount() const { return GetSize() / sizeof(uint32_t); };

		unsigned int GetSize() const { return mSize; }
		void SetSize(unsigned int val) { mSize = val; }
	private:
		RendererID mRendererID;
		unsigned int mSize;
	};
}
