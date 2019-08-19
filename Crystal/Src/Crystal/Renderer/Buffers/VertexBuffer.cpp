#include "Crpch.h"
#include "VertexBuffer.h"

#include "Platform/OpenGL/Buffers/OpenGLVertexBuffer.h"

#include "../Renderer.h"

namespace Crystal
{
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case API::OpenGL:	return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
