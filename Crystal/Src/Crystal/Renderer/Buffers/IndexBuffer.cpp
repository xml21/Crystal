#include "Crpch.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/Buffers/OpenGLIndexBuffer.h"

#include "../RendererAPI.h"
#include "../Renderer.h"

namespace Crystal
{
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case API::OpenGL:	return std::make_shared<OpenGLIndexBuffer>(indices, size);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
