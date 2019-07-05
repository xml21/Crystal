#pragma once

#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/Buffers/VertexBuffer.h"

namespace Crystal
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

	private:
		uint32_t mRendererID;
	};
}
