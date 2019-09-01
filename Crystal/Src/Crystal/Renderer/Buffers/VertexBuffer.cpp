#include "Crpch.h"
#include "VertexBuffer.h"

#include "Platform/OpenGL/Buffers/OpenGLVertexBuffer.h"

#include "Crystal/Renderer/Renderer.h"

namespace Crystal
{
	void VertexBuffer::SetData(void* Buffer, unsigned int Size, unsigned int Offset /*= 0*/)
	{

	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case API::OpenGL:	return std::make_shared<OpenGLVertexBuffer>(size);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
