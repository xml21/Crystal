#pragma once

#include "Crystal/Renderer/Buffers/IndexBuffer.h"

namespace Crystal
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual uint32_t GetCount() const { return mCount; };

	private:
		uint32_t mRendererID, mCount;
	};
}
