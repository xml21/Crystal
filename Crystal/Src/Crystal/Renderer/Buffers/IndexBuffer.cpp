#include "Crpch.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/Buffers/OpenGLIndexBuffer.h"

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal
{
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t Size)
	{
		switch (Renderer::GetAPI())
		{
			case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case API::OpenGL:	return std::make_shared<OpenGLIndexBuffer>(Size);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
